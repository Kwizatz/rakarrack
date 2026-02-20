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

#include <memory>
#include <array>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <string>

inline constexpr float D_PI = 6.283185f;
inline constexpr float PI = 3.141598f;
inline constexpr float LOG_10 = 2.302585f;
inline constexpr float LOG_2 = 0.693147f;
inline constexpr float LN2R = 1.442695041f;
inline constexpr float CNST_E = 2.71828182845905f;
inline constexpr float AMPLITUDE_INTERPOLATION_THRESHOLD = 0.0001f;
inline constexpr int FF_MAX_VOWELS = 6;
inline constexpr int FF_MAX_FORMANTS = 12;
inline constexpr int FF_MAX_SEQUENCE = 8;
inline constexpr int MAX_FILTER_STAGES = 5;
inline constexpr float RND() { return static_cast<float>(rand()) / (RAND_MAX + 1.0f); }
inline constexpr float RND1() { return static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) + 1.0f); }
inline void F2I(float f, int &i) { i = (f > 0) ? static_cast<int>(f) : static_cast<int>(f - 1.0f); }
inline float dB2rap(float dB) { return expf(dB * LOG_10 / 20.0f); }
inline float rap2dB(float rap) { return 20.0f * logf(rap) / LOG_10; }
template<typename T>
inline constexpr T CLAMP(T x, T low, T high) { return (x > high) ? high : ((x < low) ? low : x); }
inline constexpr float INTERPOLATE_AMPLITUDE(float a, float b, float x, float size) { return a + (b - a) * x / size; }
inline bool ABOVE_AMPLITUDE_THRESHOLD(float a, float b) { return (2.0f * fabsf(b - a) / (fabsf(b + a + 0.0000000001f))) > AMPLITUDE_INTERPOLATION_THRESHOLD; }
inline constexpr int POLY = 8;
inline constexpr float DENORMAL_GUARD = 1e-18f;
inline constexpr uint32_t SwapFourBytes(uint32_t data) { return ((data >> 24) & 0x000000ff) | ((data >> 8) & 0x0000ff00) | ((data << 8) & 0x00ff0000) | ((data << 24) & 0xff000000); }
inline constexpr float D_NOTE = 1.059463f;
inline constexpr float LOG_D_NOTE = 0.057762f;
inline constexpr float D_NOTE_SQRT = 1.029302f;
inline constexpr int MAX_PEAKS = 8;
inline constexpr int MAX_ALIENWAH_DELAY = 100;
inline constexpr float ATTACK = 0.175f;
inline constexpr int MAX_DELAY = 2;
inline constexpr int MAXHARMS = 8;
inline constexpr int MAX_PHASER_STAGES = 12;
inline constexpr float MAX_CHORUS_DELAY = 250.0f;
inline constexpr float LN2 = 1.0f;
inline constexpr float MUG_CORR_FACT = 0.4f;
inline constexpr float Thi = 0.67f;
inline constexpr float Tlo = -0.65f;
inline constexpr float Tlc = -0.6139445f;
inline constexpr float Thc = 0.6365834f;
inline constexpr float CRUNCH_GAIN = 100.0f;
inline constexpr float DIV_TLC_CONST = 0.002f;
inline constexpr float DIV_THC_CONST = 0.0016666f;
inline constexpr float D_FLANGE_MAX_DELAY = 0.055f;
inline constexpr float LFO_CONSTANT = 9.765625e-04f;
inline constexpr float LOG_FMAX = 10.0f;
inline constexpr float MINDEPTH = 20.0f;
inline constexpr float MAXDEPTH = 15000.0f;
inline constexpr int MAX_EQ_BANDS = 16;
inline constexpr int CLOSED = 1;
inline constexpr int OPENING = 2;
inline constexpr int OPEN = 3;
inline constexpr int CLOSING = 4;
inline constexpr float ENV_TR = 0.0001f;
inline constexpr int HARMONICS = 11;
inline constexpr int REV_COMBS = 8;
inline constexpr int REV_APS = 4;
inline constexpr int MAX_SFILTER_STAGES = 12;

union ls_pcast32 {
    float f;
    int32_t i;
};

/*
static inline float f_pow2(float x)
{
        ls_pcast32 *px, tx, lx;
        float dx;

        px = (ls_pcast32 *)&x; // store address of float as long pointer
        tx.f = (x-0.5f) + (3<<22); // temporary value for truncation
        lx.i = tx.i - 0x4b400000; // integer power of 2
        dx = x - (float)lx.i; // float remainder of power of 2

        x = 1.0f + dx * (0.6960656421638072f + // cubic apporoximation of 2^x
                   dx * (0.224494337302845f +  // for x in the range [0, 1]
                   dx * (0.07944023841053369f)));
        (*px).i += (lx.i << 23); // add integer power of 2 to exponent

        return (*px).f;
}
*/
/*
#define P2a0  1.00000534060469
#define P2a1   0.693057900547259
#define P2a2   0.239411678986933
#define P2a3   0.0532229404911678
#define P2a4   0.00686649174914722
#include <math.h>
static inline float f_pow2(float x)
{
float y,xx, intpow;
long xint = (int) fabs(ceil(x));
xx = x - ceil(x);
xint = xint<<xint;
if(x>0) intpow = (float) xint;
else intpow = 1.0f;

y = intpow*(xx*(xx*(xx*(xx*P2a4 + P2a3) + P2a2) + P2a1) + P2a0);

return y;

}
*/

//The below pow function really works & is good to 16 bits, but is it faster than math lib powf()???
//globals
#include <cmath>
static constexpr std::array<float, 5> a = { 1.00000534060469f, 0.693057900547259f, 0.239411678986933f, 0.0532229404911678f, 0.00686649174914722f };
static constexpr std::array<float, 25> pw2 = {1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f, 128.0f, 256.0f, 512.0f, 1024.0f, 2048.0f, 4096.0f, 8192.0f, 16384.0f, 32768.0f, 65536.0f, 131072.0f, 262144.0f, 524288.0f, 1048576.0f, 2097152.0f, 4194304.0f, 8388608.0f, 16777216.0f};
static constexpr std::array<float, 25> ipw2 = {1.0f, 5.0e-01f, 2.5e-01f, 1.25e-01f, 6.25e-02f, 3.125e-02f, 1.5625e-02f, 7.8125e-03f, 3.90625e-03f, 1.953125e-03f, 9.765625e-04f, 4.8828125e-04f, 2.44140625e-04f, 1.220703125e-04f, 6.103515625e-05f, 3.0517578125e-05f, 1.52587890625e-05f, 7.62939453125e-06f, 3.814697265625e-06f, 1.9073486328125e-06f, 9.5367431640625e-07f, 4.76837158203125e-07f, 2.38418579101562e-07f, 1.19209289550781e-07f, 5.96046447753906e-08f};

static inline float f_pow2(float x)
{
    float y = 0.0f;

    if(x >=24) return pw2[24];
    else if (x <= -24.0f) return ipw2[24];
    else {
        float whole =  ceilf(x);
        int xint = (int) whole;
        x = x - whole;

        if (xint>=0) {
            y = pw2[xint]*(x*(x*(x*(x*a[4] + a[3]) + a[2]) + a[1]) + a[0]);

        } else  {

            y = ipw2[-xint]*(x*(x*(x*(x*a[4] + a[3]) + a[2]) + a[1]) + a[0]);

        }

        return y;
    }

}

inline float f_exp(float x) { return f_pow2(x * LN2R); }

#include <X11/xpm.h>
#include "config.hpp"
#include <signal.h>
#include <dirent.h>
#include <search.h>
#include <sys/time.h>
#include <jack/jack.h>
#include <jack/midiport.h>
#ifdef ENABLE_MIDI
#include <alsa/asoundlib.h>
#include "MIDIConverter.hpp"
#endif
#include <FL/Fl_Preferences.H>
#include "FPreset.hpp"
#include "Reverb.hpp"
#include "Chorus.hpp"
#include "Echo.hpp"
#include "Phaser.hpp"
#include "Distorsion.hpp"
#include "EQ.hpp"
#include "Compressor.hpp"
#include "Alienwah.hpp"
#include "DynamicFilter.hpp"
#include "Pan.hpp"
#include "Harmonizer.hpp"
#include "MusicDelay.hpp"
#include "Gate.hpp"
#include "Tuner.hpp"
#include "RecognizeNote.hpp"
#include "RecChord.hpp"
#include "NewDist.hpp"
#include "APhaser.hpp"
#include "Valve.hpp"
#include "Dual_Flange.hpp"
#include "Ring.hpp"
#include "Exciter.hpp"
#include "MBDist.hpp"
#include "Arpie.hpp"
#include "Expander.hpp"
#include "Shuffle.hpp"
#include "Synthfilter.hpp"
#include "MBVvol.hpp"
#include "Convolotron.hpp"
#include "Resample.hpp"
#include "Looper.hpp"
#include "RyanWah.hpp"
#include "RBEcho.hpp"
#include "CoilCrafter.hpp"
#include "ShelfBoost.hpp"
#include "Vocoder.hpp"
#include "Sustainer.hpp"
#include "Sequence.hpp"
#include "Shifter.hpp"
#include "StompBox.hpp"
#include "Reverbtron.hpp"
#include "Echotron.hpp"
#include "StereoHarm.hpp"
#include "CompBand.hpp"
#include "Opticaltrem.hpp"
#include "Vibe.hpp"
#include "Infinity.hpp"
#include "beattracker.hpp"
extern int Pexitprogram, preset;
extern int commandline, gui;
extern int exitwithhelp, nojack;
extern int maxx_len;
extern int error_num;
extern int PERIOD;
extern int reconota;
extern int needtoloadstate;
extern int needtoloadbank;
extern int stecla;
extern int looper_lqua;
extern unsigned int SAMPLE_RATE;
extern std::array<int, POLY> note_active;
extern std::array<int, POLY> rnote;
extern std::array<int, POLY> gate;
extern std::array<int, 50> pdata;
extern float val_sum;
extern float fPERIOD;
extern float fSAMPLE_RATE;
extern float cSAMPLE_RATE;
extern std::array<float, 12> r__ratio;
extern int Wave_res_amount;
extern int Wave_up_q;
extern int Wave_down_q;
extern std::array<float, 12> freqs;
extern std::array<float, 12> lfreqs;
extern float aFreq;
extern char *s_uuid;
extern char *statefile;
extern char *filetoload;
extern char *banktoload;
extern Pixmap p;
extern Pixmap mask;
extern XWMHints *hints;

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

    std::unique_ptr<FPreset> Fpre;
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

    jack_client_t *jackclient;
    jack_options_t options;
    jack_status_t status;
    std::array<char, 64> jackcliname{};

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
    int Selected_Preset;
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
    int comemouse;
    int aconnect_MI;
    int aconnect_JA;
    int aconnect_JIA;

    int cuan_jack;
    int cuan_ijack;
    int IsCoIn;
    int Cyoin;
    int Pyoin;
    int Ccin;
    int Pcin;

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
    int font;
    int flpos;
    int upsample;
    int UpQual;
    int DownQual;
    int UpAmo;
    int J_SAMPLE_RATE;
    int J_PERIOD;
    int m_displayed;
    int Mvalue;
    std::array<int, 32> Mnumeff{};
    int OnOffC;

    int sw_stat;
    int MIDIway;
    int NumParams;
    int NumEffects;
    int relfontsize;
    int resolution;
    int sh;
    int sschema;
    int slabel_color;
    int sfore_color;
    int sback_color;
    int sleds_color;

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
    int numpi, numpo, numpa, numpmi, numpmo;
    int numpc;
    int midi_table;
    int a_bank;
    int new_bank_loaded;

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
    int EnableBackgroundImage;
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

    std::array<char, 64> Preset_Name{};
    std::array<char, 64> Author{};
    std::array<char, 128> Bank_Saved{};
    std::array<char, 128> UserRealName{};



    std::array<char, 128> MID{};
    std::array<char, 128> BankFilename{};
    std::array<char, 128> UDirFilename{};
    std::array<char, 256> BackgroundImage{};




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


    struct Preset_Bank_Struct {
        std::array<char, 64> Preset_Name{};
        std::array<char, 64> Author{};
        std::array<char, 36> Classe{};
        std::array<char, 4> Type{};
        std::array<char, 128> ConvoFiname{};
        std::array<char, 64> cInput_Gain{};
        std::array<char, 64> cMaster_Volume{};
        std::array<char, 64> cBalance{};
        float Input_Gain;
        float Master_Volume;
        float Balance;
        int Bypass;
        std::array<char, 128> RevFiname{};
        std::array<char, 128> EchoFiname{};
        std::array<std::array<int, 20>, 70> lv{};
        std::array<std::array<int, 20>, 128> XUserMIDI{};
        std::array<int, 128> XMIDIrangeMin{};
        std::array<int, 128> XMIDIrangeMax{};
    } Bank[62];


    struct MIDI_table {
        int bank;
        int preset;
    } M_table[128];

    struct Bank_Names {
        std::array<char, 64> Preset_Name{};
    } B_Names[4][62];


#ifdef ENABLE_MIDI
    // Alsa MIDI
    snd_seq_t *midi_in, *midi_out;
#endif

    struct JackPorts {
        std::array<char, 128> name{};
    } jack_po[16],jack_poi[16];




};

struct list_element {
    struct list_element *forward;
    struct list_element *backward;
    char *name;
};

#endif


