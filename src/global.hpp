/*
  rakarrack - a guitar efects software

  global.h  -  Variable Definitions and functions
  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu & Ryan Billing

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/


#ifndef DXEMU_H
#define DXEMU_H

#include "dsp_constants.hpp"
#include "PresetBank.hpp"
#include "AppConfig.hpp"

#include <signal.h>
#include <dirent.h>
#include <search.h>
#include <sys/time.h>
#include <jack/jack.h>
#include <jack/midiport.h>
#ifdef ENABLE_MIDI
#include <alsa/asoundlib.h>
#endif
#include <FL/Fl_Preferences.H>
#ifndef _WIN32
#include <X11/xpm.h>
#endif

// Forward declarations for types used via std::unique_ptr in RKR class.
class Reverb;
class Chorus;
class Echo;
class Phaser;
class Analog_Phaser;
class Distorsion;
class EQ;
class Compressor;
class DynamicFilter;
class Alienwah;
class Pan;
class Harmonizer;
class MusicDelay;
class Gate;
class NewDist;
class Tuner;
class Recognize;
class RecChord;
class Valve;
class Dflange;
class Ring;
class Exciter;
class MBDist;
class Arpie;
class Expander;
class Shuffle;
class Synthfilter;
class MBVvol;
class Convolotron;
class Resample;
class AnalogFilter;
class Looper;
class RyanWah;
class RBEcho;
class CoilCrafter;
class ShelfBoost;
class Vocoder;
class Sustainer;
class Sequence;
class Shifter;
class StompBox;
class Reverbtron;
class Echotron;
class StereoHarm;
class CompBand;
class Opticaltrem;
class Vibe;
class Infinity;
class beattracker;
class metronome;
#ifdef ENABLE_MIDI
class MIDIConverter;
#endif

// Program state globals (defined in process.cpp)
extern int Pexitprogram, preset;
extern int commandline, gui;
extern int exitwithhelp, nojack;
extern int maxx_len;
extern int error_num;
extern int needtoloadstate;
extern int needtoloadbank;
extern int stecla;
extern int looper_lqua;
extern char *s_uuid;
extern char *statefile;
extern char *filetoload;
extern char *banktoload;
#ifndef _WIN32
extern Pixmap p;
extern Pixmap mask;
extern XWMHints *hints;
#endif

// Groups JACK connection state and port info.
struct JackClient {
    jack_client_t *client{};
    jack_options_t options{};
    jack_status_t status{};
    std::array<char, 64> name{};

    // Port counts
    int num_output_ports{};
    int num_input_ports{};
    int num_aux_ports{};
    int num_midi_in_ports{};
    int num_midi_out_ports{};
    int num_pc_ports{};

    // Connection state
    int cuan_jack{};
    int cuan_ijack{};
    int IsCoIn{};
    int Cyoin{};
    int Pyoin{};
    int Ccin{};
    int Pcin{};

    // Runtime audio params
    int sample_rate{};
    int period{};

    struct Port {
        std::array<char, 128> name{};
    };
    Port output_ports[16]{};
    Port input_ports[16]{};
};

class RKR
{



public:

    RKR ();
    ~RKR ();

    void Alg (float *inl, float *inr,float *origl, float *origr ,void *);
    void Control_Gain (float *origl, float *origr);
    void Control_Volume (float *origl, float *origr);

    void Vol_Efx (int NumEffect, float volume);
    void Vol2_Efx ();
    void Vol3_Efx ();
    void cleanup_efx ();
    void midievents();
    void miramidi ();
    void calculavol (int i);
    void Bank_to_Preset (int Num);
    void Preset_to_Bank (int i);
    void Actualizar_Audio ();
    void loadfile (char *filename);
    void getbuf (char *buf, int j);
    void putbuf (char *buf, int j);
    void savefile (char *filename);
    void SaveIntPreset(int num, char *name);
    void DelIntPreset(int num, char *name);
    void MergeIntPreset(char *filename);
    void loadmiditable (char *filename);
    void savemiditable (char *filename);
    void loadskin (char *filename);
    void saveskin (char *filename);
    int loadbank (char *filename);
    void loadnames();
    int savebank (char *filename);
    void ConvertOldFile(char *filename);
    void ConvertReverbFile(char * filename);
    void dump_preset_names ();
    void New ();
    void New_Bank ();
    void Adjust_Upsample();
    void add_metro();
    void init_rkr ();
    int Message (int prio, const char *labelwin, const char *message_text);
    char *PrefNom (const char *dato);
    void EQ1_setpreset (int npreset);
    void EQ2_setpreset (int npreset);
    int Cabinet_setpreset (int npreset);
    void InitMIDI ();
    void ConnectMIDI ();
    void ActiveUn(int value);
    void ActOnOff();
    void jack_process_midievents (jack_midi_event_t *midievent);
    void process_midi_controller_events(int parameter, int value);
    int ret_Tempo(int value);
    int ret_LPF(int value);
    int ret_HPF(int value);
    void Conecta ();
    void disconectaaconnect ();
    void conectaaconnect ();
    int BigEndian();
    void fix_endianess();
    void copy_IO();
    void convert_IO();
    int CheckOldBank(char *filename);
    int Get_Bogomips();
    int checkonoff(int value);
    int TapTempo();
    void TapTempo_Timeout(int state);
    void Update_tempo();
    int checkforaux();
    void Error_Handle(int num);
    void update_freqs(float val);

    std::unique_ptr<Reverb> efx_Rev;
    std::unique_ptr<Chorus> efx_Chorus;
    std::unique_ptr<Chorus> efx_Flanger;
    std::unique_ptr<Phaser> efx_Phaser;
    std::unique_ptr<Analog_Phaser> efx_APhaser;
    std::unique_ptr<EQ> efx_EQ1;
    std::unique_ptr<EQ> efx_EQ2;
    std::unique_ptr<Echo> efx_Echo;
    std::unique_ptr<Distorsion> efx_Distorsion;
    std::unique_ptr<Distorsion> efx_Overdrive;
    std::unique_ptr<Compressor> efx_Compressor;
    std::unique_ptr<DynamicFilter> efx_WhaWha;
    std::unique_ptr<Alienwah> efx_Alienwah;
    std::unique_ptr<EQ> efx_Cabinet;
    std::unique_ptr<Pan> efx_Pan;
    std::unique_ptr<Harmonizer> efx_Har;
    std::unique_ptr<MusicDelay> efx_MusDelay;
    std::unique_ptr<Gate> efx_Gate;
    std::unique_ptr<NewDist> efx_NewDist;
    std::unique_ptr<Tuner> efx_Tuner;
#ifdef ENABLE_MIDI
    std::unique_ptr<MIDIConverter> efx_MIDIConverter;
#endif
    std::unique_ptr<metronome> M_Metronome;
    std::unique_ptr<beattracker> beat;

    std::unique_ptr<Recognize> RecNote;
    std::unique_ptr<RecChord> RC;
    std::unique_ptr<Compressor> efx_FLimiter;
    std::unique_ptr<Valve> efx_Valve;
    std::unique_ptr<Dflange> efx_DFlange;
    std::unique_ptr<Ring> efx_Ring;
    std::unique_ptr<Exciter> efx_Exciter;
    std::unique_ptr<MBDist> efx_MBDist;
    std::unique_ptr<Arpie> efx_Arpie;
    std::unique_ptr<Expander> efx_Expander;
    std::unique_ptr<Synthfilter> efx_Synthfilter;
    std::unique_ptr<Shuffle> efx_Shuffle;
    std::unique_ptr<MBVvol> efx_MBVvol;
    std::unique_ptr<Convolotron> efx_Convol;
    std::unique_ptr<Resample> U_Resample;
    std::unique_ptr<Resample> D_Resample;
    std::unique_ptr<Resample> A_Resample;
    std::unique_ptr<AnalogFilter> DC_Offsetl;
    std::unique_ptr<AnalogFilter> DC_Offsetr;
    std::unique_ptr<Looper> efx_Looper;
    std::unique_ptr<RyanWah> efx_RyanWah;
    std::unique_ptr<RBEcho> efx_RBEcho;
    std::unique_ptr<CoilCrafter> efx_CoilCrafter;
    std::unique_ptr<ShelfBoost> efx_ShelfBoost;
    std::unique_ptr<Vocoder> efx_Vocoder;
    std::unique_ptr<Sustainer> efx_Sustainer;
    std::unique_ptr<Sequence> efx_Sequence;
    std::unique_ptr<Shifter> efx_Shifter;
    std::unique_ptr<StompBox> efx_StompBox;
    std::unique_ptr<Reverbtron> efx_Reverbtron;
    std::unique_ptr<Echotron> efx_Echotron;
    std::unique_ptr<StereoHarm> efx_StereoHarm;
    std::unique_ptr<CompBand> efx_CompBand;
    std::unique_ptr<Opticaltrem> efx_Opticaltrem;
    std::unique_ptr<Vibe> efx_Vibe;
    std::unique_ptr<Infinity> efx_Infinity;

    JackClient jack;

    int db6booster;
    int jdis;
    int jshut;
    int DC_Offset;
    int Bypass;
    int MIDIConverter_Bypass;
    int Metro_Bypass;
    int Tuner_Bypass;
    int Tap_Bypass;
    int ACI_Bypass;
    int Reverb_Bypass;
    int Chorus_Bypass;
    int Flanger_Bypass;
    int Phaser_Bypass;
    int Overdrive_Bypass;
    int Distorsion_Bypass;
    int Echo_Bypass;
    int EQ1_Bypass;
    int EQ2_Bypass;
    int Compressor_Bypass;
    int WhaWha_Bypass;
    int Alienwah_Bypass;
    int Cabinet_Bypass;
    int Pan_Bypass;
    int Harmonizer_Bypass;
    int MusDelay_Bypass;
    int Gate_Bypass;
    int NewDist_Bypass;
    int APhaser_Bypass;
    int Valve_Bypass;
    int DFlange_Bypass;
    int Ring_Bypass;
    int Exciter_Bypass;
    int MBDist_Bypass;
    int Arpie_Bypass;
    int Expander_Bypass;
    int Shuffle_Bypass;
    int Synthfilter_Bypass;
    int MBVvol_Bypass;
    int Convol_Bypass;
    int Looper_Bypass;
    int RyanWah_Bypass;
    int RBEcho_Bypass;
    int CoilCrafter_Bypass;
    int ShelfBoost_Bypass;
    int Vocoder_Bypass;
    int Sustainer_Bypass;
    int Sequence_Bypass;
    int Shifter_Bypass;
    int StompBox_Bypass;
    int Reverbtron_Bypass;
    int Echotron_Bypass;
    int StereoHarm_Bypass;
    int CompBand_Bypass;
    int Opticaltrem_Bypass;
    int Vibe_Bypass;
    int Infinity_Bypass;
    int Bypass_B;
    int Reverb_B;
    int Chorus_B;
    int Flanger_B;
    int Phaser_B;
    int APhaser_B;
    int DFlange_B;
    int Overdrive_B;
    int Distorsion_B;
    int Echo_B;
    int EQ1_B;
    int EQ2_B;
    int Compressor_B;
    int WhaWha_B;
    int Alienwah_B;
    int Cabinet_B;
    int Pan_B;
    int Harmonizer_B;
    int MusDelay_B;
    int Gate_B;
    int NewDist_B;
    int Valve_B;
    int Ring_B;
    int Exciter_B;
    int MBDist_B;
    int Arpie_B;
    int Expander_B;
    int Shuffle_B;
    int Synthfilter_B;
    int MBVvol_B;
    int Convol_B;
    int Looper_B;
    int RyanWah_B;
    int RBEcho_B;
    int CoilCrafter_B;
    int ShelfBoost_B;
    int Vocoder_B;
    int Sustainer_B;
    int Sequence_B;
    int Shifter_B;
    int StompBox_B;
    int Reverbtron_B;
    int Echotron_B;
    int StereoHarm_B;
    int CompBand_B;
    int Opticaltrem_B;
    int Vibe_B;
    int Infinity_B;

    int Cabinet_Preset;
    std::array<std::array<int, 20>, 70> lv{};
    std::array<int, 16> saved_order{};
    std::array<int, 16> efx_order{};
    std::array<int, 16> new_order{};
    std::array<int, 60> availables{};
    std::array<int, 12> active{};
    int MidiCh;
    int HarCh;
    int init_state;
    int actuvol;
    int help_displayed;
    int modified;
    int autoassign;

    // bank of flags telling GUI which midi controlled items to update
    std::array<int, 500> Mcontrol{};
    // flag telling GUI that Mcontrol has at least one set flag
    int RControl;
    int ControlGet;
    int CountWait;
    std::array<std::array<int, 20>, 128> XUserMIDI{};

    int eff_filter;
    int Har_Down;
    int Har_U_Q;
    int Har_D_Q;
    int Rev_Down;
    int Rev_U_Q;
    int Rev_D_Q;
    int Con_Down;
    int Con_U_Q;
    int Con_D_Q;
    int Shi_Down;
    int Shi_U_Q;
    int Shi_D_Q;
    int Seq_Down;
    int Seq_U_Q;
    int Seq_D_Q;
    int Voc_Down;
    int Voc_U_Q;
    int Voc_D_Q;
    int Ste_Down;
    int Ste_U_Q;
    int Ste_D_Q;

    int Metro_Vol;
    int M_Metro_Sound;
    int deachide;
    int upsample;
    int UpQual;
    int DownQual;
    int UpAmo;
    int m_displayed;
    int Mvalue;
    std::array<int, 32> Mnumeff{};
    int OnOffC;

    int sw_stat;
    int MIDIway;
    int NumParams;
    int NumEffects;
    int sh;

    int have_signal;
    int OnCounter;
    int t_periods;

    //   Recognize

    int last;

    // Harmonizer
    int HarQual;
    int SteQual;

    // Tap Tempo

    int tempocnt;
    int Tap_Display;
    int Tap_Selection;
    int Tap_TempoSet;
    int Tap_SetValue;
    int t_timeout;
    int jt_state;
    int Tap_Updated;
    int note_old;
    int cents_old;

    int cpufp;
    int mess_dis;
    int midi_table;

    int Aux_Gain;
    int Aux_Threshold;
    int Aux_MIDI;
    int Aux_Minimum;
    int Aux_Maximum;
    int Aux_Source;
    int last_auxvalue;
    int ena_tool;
    int VocBands;
    int RCOpti;

    int M_Metro_Tempo;
    int M_Metro_Bar;
    int mtc_counter;
    int ML_filter;
    std::array<int, 150> ML_clist{};

    long Tap_time_Init;

    double Tap_timeB;
    double Tap_timeC;
    double jt_tempo;

    std::array<double, 6> tempobuf{};


    double u_down;
    double u_up;

    timeval timeA;

    float booster;
    float cpuload;
    float rtrig;

    std::vector<float> efxoutl;
    std::vector<float> efxoutr;
    std::vector<float> auxdata;
    std::vector<float> auxresampled;
    std::vector<float> anall;
    std::vector<float> analr;
    std::vector<float> smpl;
    std::vector<float> smpr;
    std::vector<float> denormal;
    std::vector<float> m_ticks;

    float Master_Volume;
    float Input_Gain;
    float Fraction_Bypass;
    float Log_I_Gain;
    float Log_M_Volume;
    float M_Metro_Vol;


    float old_il_sum;
    float old_ir_sum;
    float old_vl_sum;
    float old_vr_sum;
    float val_vl_sum;
    float val_vr_sum;
    float val_il_sum;
    float val_ir_sum;
    float old_a_sum;
    float val_a_sum;


    float bogomips;
    float looper_size;

// Tunner

    float nfreq_old;
    float afreq_old;

    std::array<char, 128> tmpprefname{};



    std::array<char, 128> MID{};




    struct Effects_Names {
        std::array<char, 24> Nom{};
        int Pos;
        int Type;

    } efx_names[70];

    struct Effects_Params {
        std::array<char, 32> Nom{};
        int Ato;
        int Effect;
    } efx_params[500];


    PresetBank presets;
    AppConfig config;


#ifdef ENABLE_MIDI
    // Alsa MIDI
    snd_seq_t *midi_in, *midi_out;
#endif




};

struct list_element {
    struct list_element *forward;
    struct list_element *backward;
    char *name;
};

#endif


