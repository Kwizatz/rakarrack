/*
  rakarrack - a guitar efects software

  jack.C  -   jack I/O
  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.

  You should have received a copy of the GNU General Public License
(version2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#include <chrono>
#include <thread>
#include "config.hpp"
#include <jack/jack.h>
#include <jack/midiport.h>
#ifdef HAVE_JACK_TRANSPORT
#include <jack/transport.h>
#endif
#include "jack.hpp"
#include "global.hpp"
#include "EngineController.hpp"
#include "AllEffects.hpp"
#include "Tuner.hpp"
#ifdef ENABLE_MIDI
#include "MIDIConverter.hpp"
#endif

#include <sndfile.h>

#include <atomic>
#include <cstdlib>
#include <vector>

// Diagnostic tap. Set RAKARRACK_CAPTURE to a number of seconds to record what
// the process callback is handed and what it hands back, written out at
// shutdown. This distinguishes a bad input from bad processing, which is not
// something the meters can tell you, and counts the xruns and short buffers
// that would show up as gaps in the sound rather than as a level problem.
namespace
{
struct CaptureTap
{
    std::vector<float> in;      ///< interleaved stereo, as received
    std::vector<float> out;     ///< interleaved stereo, as returned
    std::size_t frames = 0;     ///< frames stored so far
    std::size_t capacity = 0;   ///< frames the buffers can hold
    bool active = false;
};

CaptureTap g_capture;
std::atomic<int> g_xruns{0};
std::atomic<unsigned> g_shortBlocks{0};
std::atomic<unsigned> g_expectedFrames{0};

// Callback duration. JACK reports a late client without saying how late, and
// that is the difference between an engine too slow to hold the deadline and a
// deadline set too short for it.
std::atomic<long long> g_procMaxNs{0};
std::atomic<long long> g_procTotalNs{0};
std::atomic<long long> g_procCalls{0};

// Gap between successive callbacks. It should equal the period; if it does not,
// the cycle was already late before any of our code ran.
std::chrono::steady_clock::time_point g_lastStart{};
std::atomic<long long> g_gapMaxNs{0};
std::atomic<long long> g_gapOverBudget{0};

int countXrun(void*)
{
    g_xruns.fetch_add(1, std::memory_order_relaxed);
    return 0;
}

/// Recount what each port is connected to, for the GUI's port display.
///
/// jack_port_connected() is not realtime safe. Measured against a running
/// server it averages well under a microsecond but peaks at 6.7 ms, which is
/// longer than an entire 256-frame period, and the process callback was making
/// seven of these calls every cycle. That is what JACK was reporting as "client
/// was not finished": the engine itself uses about 2% of a period, so the
/// deadline was being missed inside the JACK API rather than in the DSP.
void refreshPortCounts();

void onPortsConnected(jack_port_id_t, jack_port_id_t, int, void*)
{
    refreshPortCounts();
}

void onPortRegistered(jack_port_id_t, int, void*)
{
    refreshPortCounts();
}

void writeTap(const char* path, const std::vector<float>& data, std::size_t frames,
              int rate)
{
    if (frames == 0)
        return;
    SF_INFO info{};
    info.samplerate = rate;
    info.channels = 2;
    info.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    if (SNDFILE* f = sf_open(path, SFM_WRITE, &info))
    {
        sf_writef_float(f, data.data(), static_cast<sf_count_t>(frames));
        sf_close(f);
        printf("capture: wrote %s (%zu frames)\n", path, frames);
    }
}
} // namespace


RKR *JackOUT;

jack_client_t *jackclient;
jack_port_t *outport_left, *outport_right;
jack_port_t *inputport_left, *inputport_right, *inputport_aux;
jack_port_t *jack_midi_in, *jack_midi_out;
void *dataout;
int jackprocess (jack_nframes_t nframes, void *arg);

namespace
{
void refreshPortCounts()
{
    if (JackOUT == nullptr)
        return;

    const int in = jack_port_connected(inputport_left)
                 + jack_port_connected(inputport_right);
    const int out = jack_port_connected(outport_left)
                  + jack_port_connected(outport_right);
    const int aux = jack_port_connected(inputport_aux);
    const int midiIn = jack_port_connected(jack_midi_in);
    const int midiOut = jack_port_connected(jack_midi_out);

    if (in != JackOUT->jack.num_input_ports
        || out != JackOUT->jack.num_output_ports
        || aux != JackOUT->jack.num_aux_ports
        || midiIn != JackOUT->jack.num_midi_in_ports
        || midiOut != JackOUT->jack.num_midi_out_ports)
    {
        JackOUT->jack.num_input_ports = in;
        JackOUT->jack.num_output_ports = out;
        JackOUT->jack.num_aux_ports = aux;
        JackOUT->jack.num_midi_in_ports = midiIn;
        JackOUT->jack.num_midi_out_ports = midiOut;
        JackOUT->jack.num_pc_ports = 1;
    }
}
} // namespace

int
JACKstart (RKR * rkr_, jack_client_t * jackclient_)
{

    JackOUT = rkr_;
    jackclient = jackclient_;

#ifdef HAVE_JACK_TRANSPORT
    jack_set_sync_callback(jackclient, timebase, nullptr);
#endif
    jack_set_process_callback (jackclient, jackprocess, 0);

    jack_on_shutdown (jackclient, jackshutdown, 0);

    jack_set_xrun_callback (jackclient, countXrun, 0);

    jack_set_port_connect_callback (jackclient, onPortsConnected, 0);
    jack_set_port_registration_callback (jackclient, onPortRegistered, 0);

    if (const char* secs = getenv("RAKARRACK_CAPTURE"))
    {
        const int seconds = atoi(secs);
        if (seconds > 0)
        {
            const auto rate = static_cast<std::size_t>(jack_get_sample_rate(jackclient));
            g_capture.capacity = rate * static_cast<std::size_t>(seconds);
            g_capture.in.assign(g_capture.capacity * 2, 0.0F);
            g_capture.out.assign(g_capture.capacity * 2, 0.0F);
            g_capture.active = true;
            printf("capture: recording %d s of input and output\n", seconds);
        }
    }

    g_expectedFrames.store(static_cast<unsigned>(jack_get_buffer_size(jackclient)),
                           std::memory_order_relaxed);

    // Worth stating plainly. Which server you reached decides whether capture
    // and playback share a clock, and the rate is the quickest way to tell one
    // server from another when several could be running.
    printf("jack: connected at %u Hz, buffer %u frames (%.1f ms per period)\n",
           jack_get_sample_rate(jackclient), jack_get_buffer_size(jackclient),
           1000.0 * jack_get_buffer_size(jackclient) / jack_get_sample_rate(jackclient));



    inputport_left =
        jack_port_register (jackclient, "in_1", JACK_DEFAULT_AUDIO_TYPE,
                            JackPortIsInput, 0);
    inputport_right =
        jack_port_register (jackclient, "in_2", JACK_DEFAULT_AUDIO_TYPE,
                            JackPortIsInput, 0);

    inputport_aux =
        jack_port_register (jackclient, "aux", JACK_DEFAULT_AUDIO_TYPE,
                            JackPortIsInput, 0);

    outport_left =
        jack_port_register (jackclient, "out_1", JACK_DEFAULT_AUDIO_TYPE,
                            JackPortIsOutput, 0);
    outport_right =
        jack_port_register (jackclient, "out_2", JACK_DEFAULT_AUDIO_TYPE,
                            JackPortIsOutput, 0);

    jack_midi_in =
        jack_port_register(jackclient, "in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);

    jack_midi_out =
        jack_port_register(jackclient, "MC out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);


    if (jack_activate (jackclient)) {
        fprintf (stderr, "Cannot activate jack client.\n");
        return (2);
    };

    if ((JackOUT->config.aconnect_JA) && (!needtoloadstate)) {

        for (int i = 0; i < JackOUT->jack.cuan_jack; i += 2) {
            jack_connect (jackclient, jack_port_name (outport_left),
                          JackOUT->jack.output_ports[i].name.data());
            jack_connect (jackclient, jack_port_name (outport_right),
                          JackOUT->jack.output_ports[i + 1].name.data());
        }
    }

    if ((JackOUT->config.aconnect_JIA) && (!needtoloadstate)) {

        if(JackOUT->jack.cuan_ijack == 1) {
            jack_connect (jackclient,JackOUT->jack.input_ports[0].name.data(),jack_port_name(inputport_left));
            jack_connect (jackclient,JackOUT->jack.input_ports[0].name.data(), jack_port_name(inputport_right));
        }

        else {
            for (int i = 0; i < JackOUT->jack.cuan_ijack; i += 2) {
                jack_connect (jackclient,JackOUT->jack.input_ports[i].name.data(), jack_port_name (inputport_left));
                jack_connect (jackclient,JackOUT->jack.input_ports[i + 1].name.data(),jack_port_name (inputport_right));
            }
        }

    }



    refreshPortCounts();

    return 3;

};



int
jackprocess (jack_nframes_t nframes, [[maybe_unused]] void *arg)
{
    const auto processStart = std::chrono::steady_clock::now();

#ifdef HAVE_JACK_TRANSPORT
    jack_position_t pos;
    jack_transport_state_t astate;
#endif

    jack_default_audio_sample_t *outl = (jack_default_audio_sample_t *)
                                        jack_port_get_buffer (outport_left, nframes);
    jack_default_audio_sample_t *outr = (jack_default_audio_sample_t *)
                                        jack_port_get_buffer (outport_right, nframes);


    jack_default_audio_sample_t *inl = (jack_default_audio_sample_t *)
                                       jack_port_get_buffer (inputport_left, nframes);
    jack_default_audio_sample_t *inr = (jack_default_audio_sample_t *)
                                       jack_port_get_buffer (inputport_right, nframes);

    jack_default_audio_sample_t *aux = (jack_default_audio_sample_t *)
                                       jack_port_get_buffer (inputport_aux, nframes);


    JackOUT->cpuload = jack_cpu_load(jackclient);


#ifdef HAVE_JACK_TRANSPORT
    if((JackOUT->Tap_Bypass) && (JackOUT->Tap_Selection == 2)) {
        astate = jack_transport_query(jackclient, &pos);
        if(astate >0) {
            if (JackOUT->jt_tempo != pos.beats_per_minute)
                actualiza_tap(pos.beats_per_minute);
        }

        if(JackOUT->Looper_Bypass) {
            if((astate != JackOUT->jt_state) && (astate==0)) {
                JackOUT->jt_state=astate;
                JackOUT->efx_Looper->changepar(2,1);
                stecla=5;
            }

            if((astate != JackOUT->jt_state) && (astate == 3)) {
                JackOUT->jt_state=astate;
                JackOUT->efx_Looper->changepar(1,1);
                stecla=5;
            }

        }
    }
#endif




#ifdef ENABLE_MIDI
    float *data = (float *)jack_port_get_buffer(jack_midi_in, nframes);
    int count = jack_midi_get_event_count(data);
    jack_midi_event_t midievent;

    dataout = jack_port_get_buffer(jack_midi_out, nframes);
    jack_midi_clear_buffer(dataout);


    for (int i = 0; i < count; ++i) {
        jack_midi_event_get(&midievent, data, i);
        JackOUT->jack_process_midievents(&midievent);
    }

    for (int i=0; i<=JackOUT->efx_MIDIConverter->ev_count; ++i) {
        jack_midi_event_write(dataout,
                              JackOUT->efx_MIDIConverter->Midi_event[i].time,
                              JackOUT->efx_MIDIConverter->Midi_event[i].dataloc,
                              JackOUT->efx_MIDIConverter->Midi_event[i].len);
    }

    JackOUT->efx_MIDIConverter->moutdatasize = 0;
    JackOUT->efx_MIDIConverter->ev_count = 0;
#endif


    memcpy (JackOUT->efxoutl.data(), inl,
            sizeof (jack_default_audio_sample_t) * nframes);
    memcpy (JackOUT->efxoutr.data(), inr,
            sizeof (jack_default_audio_sample_t) * nframes);
    memcpy (JackOUT->auxdata.data(), aux,
            sizeof (jack_default_audio_sample_t) * nframes);




    JackOUT->Alg (JackOUT->efxoutl.data(), JackOUT->efxoutr.data(), inl, inr ,0);

    // ── Push telemetry to GUI via lock-free ring buffers ───────────
    if (JackOUT->m_controller)
    {
        // Audio levels
        AudioLevels levels;
        levels.input_left      = JackOUT->val_il_sum;
        levels.input_right     = JackOUT->val_ir_sum;
        levels.output_left     = JackOUT->val_vl_sum;
        levels.output_right    = JackOUT->val_vr_sum;
        levels.cpu_load        = JackOUT->cpuload;
        levels.have_signal     = JackOUT->have_signal;
        JackOUT->m_controller->pushLevels(levels);

        // Tuner data (when active)
        if (JackOUT->Tuner_Bypass && JackOUT->efx_Tuner)
        {
            TunerData tuner;
            tuner.note_index  = JackOUT->efx_Tuner->note_actual;
            tuner.nearest_freq = JackOUT->efx_Tuner->nfreq;
            tuner.actual_freq  = JackOUT->efx_Tuner->afreq;
            tuner.cents        = static_cast<float>(JackOUT->efx_Tuner->cents);
            if (tuner.note_index >= 0 && tuner.note_index < 12
                && JackOUT->efx_Tuner->notes)
            {
                snprintf(tuner.note_name, sizeof(tuner.note_name),
                         "%s", JackOUT->efx_Tuner->notes[tuner.note_index]);
            }
            JackOUT->m_controller->pushTuner(tuner);
        }

        // Tap tempo display flag
        if (JackOUT->Tap_Display)
        {
            TapTempoStatus tap;
            tap.display_flag = JackOUT->Tap_Display;
            tap.tempo_bpm    = static_cast<float>(JackOUT->Tap_TempoSet);
            JackOUT->m_controller->pushTapTempo(tap);
            JackOUT->Tap_Display = 0;
        }
    }

    memcpy (outl, JackOUT->efxoutl.data(),
            sizeof (jack_default_audio_sample_t) * nframes);
    memcpy (outr, JackOUT->efxoutr.data(),
            sizeof (jack_default_audio_sample_t) * nframes);

    if (nframes != g_expectedFrames.load(std::memory_order_relaxed))
        g_shortBlocks.fetch_add(1, std::memory_order_relaxed);

    if (g_capture.active && g_capture.frames < g_capture.capacity)
    {
        const std::size_t room = g_capture.capacity - g_capture.frames;
        const std::size_t take = nframes < room ? nframes : room;
        float* dstIn = g_capture.in.data() + g_capture.frames * 2;
        float* dstOut = g_capture.out.data() + g_capture.frames * 2;
        for (std::size_t i = 0; i < take; ++i)
        {
            dstIn[i * 2]      = inl[i];
            dstIn[i * 2 + 1]  = inr[i];
            dstOut[i * 2]     = outl[i];
            dstOut[i * 2 + 1] = outr[i];
        }
        g_capture.frames += take;

        // Headless has no window to close, and a killed process never reaches
        // the shutdown report, so let the requested duration end the run.
        if (g_capture.frames >= g_capture.capacity && gui == 0)
            Pexitprogram = 1;
    }

    {
        const auto now = std::chrono::steady_clock::now();
        if (g_lastStart.time_since_epoch().count() != 0)
        {
            const auto gap = std::chrono::duration_cast<std::chrono::nanoseconds>(
                                 processStart - g_lastStart).count();
            long long prevGap = g_gapMaxNs.load(std::memory_order_relaxed);
            while (gap > prevGap
                   && !g_gapMaxNs.compare_exchange_weak(prevGap, gap,
                                                        std::memory_order_relaxed))
                ;
            const long long budgetNs = 1000000000LL * g_expectedFrames.load()
                                     / (JackOUT ? JackOUT->jack.sample_rate : 44100);
            if (gap > budgetNs * 3 / 2)
                g_gapOverBudget.fetch_add(1, std::memory_order_relaxed);
        }
        g_lastStart = processStart;

        const auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                            now - processStart).count();
        g_procTotalNs.fetch_add(ns, std::memory_order_relaxed);
        g_procCalls.fetch_add(1, std::memory_order_relaxed);
        long long prev = g_procMaxNs.load(std::memory_order_relaxed);
        while (ns > prev
               && !g_procMaxNs.compare_exchange_weak(prev, ns,
                                                     std::memory_order_relaxed))
            ;
    }

    return 0;

};


void
JACKfinish ()
{
    printf("jack: %d xruns, %u odd-sized blocks (buffer size %u)\n",
           g_xruns.load(), g_shortBlocks.load(), g_expectedFrames.load());

    const int rate = static_cast<int>(jack_get_sample_rate(jackclient));

    if (const long long calls = g_procCalls.load(); calls > 0)
    {
        const double budgetMs = 1000.0 * g_expectedFrames.load() / rate;
        const double meanMs = g_procTotalNs.load() / 1e6 / static_cast<double>(calls);
        const double maxMs = g_procMaxNs.load() / 1e6;
        printf("jack: process took %.3f ms mean, %.3f ms worst, of %.3f ms available"
               " (%.0f%% mean, %.0f%% worst)\n",
               meanMs, maxMs, budgetMs,
               100.0 * meanMs / budgetMs, 100.0 * maxMs / budgetMs);
        printf("jack: cycle arrived %.3f ms late at worst, %lld of %lld cycles"
               " more than half a period apart\n",
               g_gapMaxNs.load() / 1e6 - budgetMs, g_gapOverBudget.load(), calls);
    }

    // Close first. The process callback is still running until this returns,
    // and it appends to the same buffers and counter the writes below read,
    // which raced: the second file came out 1024 frames longer than the first.
    jack_client_close (jackclient);

    if (g_capture.active)
    {
        writeTap("capture_in.wav", g_capture.in, g_capture.frames, rate);
        writeTap("capture_out.wav", g_capture.out, g_capture.frames, rate);
    }

    std::this_thread::sleep_for(std::chrono::microseconds(1000));
};



void
jackshutdown ([[maybe_unused]] void *arg)
{
    if (gui == 0)
        printf ("Jack Shut Down, sorry.\n");
    else
        JackOUT->jshut=1;

};



#ifdef HAVE_JACK_TRANSPORT
int
timebase(jack_transport_state_t state, jack_position_t *pos, [[maybe_unused]] void *arg)
{

    JackOUT->jt_state=state;


    if((JackOUT->Tap_Bypass) && (JackOUT->Tap_Selection == 2)) {
        if((state > 0) && (pos->beats_per_minute > 0)) {
            JackOUT->jt_tempo=pos->beats_per_minute;
            JackOUT->Tap_TempoSet = lrint(JackOUT->jt_tempo);
            JackOUT->Update_tempo();
            JackOUT->Tap_Display=1;
            if((JackOUT->Looper_Bypass) && (state==3)) {
                JackOUT->efx_Looper->changepar(1,1);
                stecla=5;
            }
        }
    }

    return(1);

}
#endif

void
actualiza_tap(double val)
{
    JackOUT->jt_tempo=val;
    JackOUT->Tap_TempoSet = lrint(JackOUT->jt_tempo);
    JackOUT->Update_tempo();
    JackOUT->Tap_Display=1;
}
