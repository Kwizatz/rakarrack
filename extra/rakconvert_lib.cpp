/*
 * rakconvert library - Convert old-format rakarrack bank files
 * to the current format.
 *
 * Copyright (C) 2008-2010 Josep Andreu
 * Copyright (C) 2010 Ryan Billing
 * Copyright (C) 2010 Douglas McClendon
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation.
 */

#include "rakconvert_lib.hpp"

#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>

#define SwapFourBytes(data) \
    ( (((data) >> 24) & 0x000000ff) | (((data) >> 8) & 0x0000ff00) | \
      (((data) << 8) & 0x00ff0000) | (((data) << 24) & 0xff000000) )

namespace {

// Portable fopen wrapper — uses fopen_s on MSVC, fopen elsewhere.
inline FILE* portable_fopen(const char* filename, const char* mode)
{
#ifdef _MSC_VER
    FILE* fp = nullptr;
    if (fopen_s(&fp, filename, mode) != 0)
        fp = nullptr;
    return fp;
#else
    return std::fopen(filename, mode);
#endif
}

// Portable sscanf for numeric-only format strings.
#ifdef _MSC_VER
#define LOCAL_SSCANF sscanf_s
#else
#define LOCAL_SSCANF sscanf
#endif

// ---- Data structures for old bank format ----

struct ML
{
    int XUserMIDI[128][20];
};

struct Preset_Bank_Struct
{
    char Preset_Name[64];
    char Author[64];
    char Classe[36];
    char Type[4];
    char Reserva[64];
    char Reserva1[64];
    float Input_Gain;
    float Master_Volume;
    int Bypass;
    int lv[24][22];
};

// ---- Data structures for new bank format ----

struct New_Bank_Struct
{
    char Preset_Name[64];
    char Author[64];
    char Classe[36];
    char Type[4];
    char ConvoFiname[128];
    char cInput_Gain[64];
    char cMaster_Volume[64];
    char cBalance[64];
    float Input_Gain;
    float Master_Volume;
    float Balance;
    int Bypass;
    char RevFiname[128];
    char ReservedFiname[128];
    int lv[70][20];
    int XUserMIDI[128][20];
    int XMIDIrangeMin[128];
    int XMIDIrangeMax[128];
};

// ---- Module-local state ----

ML PML[82];
Preset_Bank_Struct Bank[82];
New_Bank_Struct NewBank[62];

// ---- Helper functions ----

int BigEndian()
{
    long one = 1;
    return !(*((char *)(&one)));
}

void copy_IO()
{
    for (int i = 0; i < 62; i++) {
        memset(NewBank[i].cInput_Gain, 0, sizeof(NewBank[i].cInput_Gain));
        snprintf(NewBank[i].cInput_Gain, sizeof(NewBank[i].cInput_Gain), "%f", NewBank[i].Input_Gain);
        memset(NewBank[i].cMaster_Volume, 0, sizeof(NewBank[i].cMaster_Volume));
        snprintf(NewBank[i].cMaster_Volume, sizeof(NewBank[i].cMaster_Volume), "%f", NewBank[i].Master_Volume);
        memset(NewBank[i].cBalance, 0, sizeof(NewBank[i].cBalance));
        snprintf(NewBank[i].cBalance, sizeof(NewBank[i].cBalance), "%f", NewBank[i].Balance);
    }
}

void convert_IO()
{
    for (int i = 0; i < 82; i++) {
        LOCAL_SSCANF(Bank[i].Reserva, "%f", &Bank[i].Input_Gain);
        if (Bank[i].Input_Gain == 0.0) Bank[i].Input_Gain = 0.5f;

        LOCAL_SSCANF(Bank[i].Reserva1, "%f", &Bank[i].Master_Volume);
        if (Bank[i].Master_Volume == 0.0) Bank[i].Master_Volume = 0.5f;
    }
}

void old_fix_endianess()
{
    unsigned int data;
    for (int i = 0; i < 82; i++) {
        data = Bank[i].Bypass;
        data = SwapFourBytes(data);
        Bank[i].Bypass = data;

        for (int j = 0; j < 24; j++) {
            for (int k = 0; k < 22; k++) {
                data = Bank[i].lv[j][k];
                data = SwapFourBytes(data);
                Bank[i].lv[j][k] = data;
            }
        }
    }
}

void fix_endianess()
{
    unsigned int data;
    for (int i = 0; i < 62; i++) {
        data = NewBank[i].Bypass;
        data = SwapFourBytes(data);
        NewBank[i].Bypass = data;

        for (int j = 0; j < 70; j++) {
            for (int k = 0; k < 20; k++) {
                data = NewBank[i].lv[j][k];
                data = SwapFourBytes(data);
                NewBank[i].lv[j][k] = data;
            }
        }

        for (int j = 0; j < 128; j++) {
            for (int k = 0; k < 20; k++) {
                data = NewBank[i].XUserMIDI[j][k];
                data = SwapFourBytes(data);
                NewBank[i].XUserMIDI[j][k] = data;
            }
        }
    }
}

void New_Bank_Init()
{
    int presets[42][16] = {
        // Reverb
        {80, 64, 63, 24, 0, 0, 0, 85, 5, 83, 1, 64, 0, 0, 0, 0},
        // Echo
        {67, 64, 35, 64, 30, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        // Chorus
        {64, 64, 50, 0, 0, 90, 40, 85, 64, 119, 0, 0, 0, 0, 0, 0},
        // Flanger
        {64, 64, 57, 0, 0, 60, 23, 3, 62, 0, 0, 0, 0, 0, 0, 0},
        // Phaser
        {64, 64, 36, 0, 0, 64, 110, 64, 1, 0, 0, 20, 0, 0, 0, 0},
        // Overdrive
        {84, 64, 35, 56, 40, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0},
        // Distorsion
        {0, 64, 0, 87, 14, 6, 0, 80, 30, 0, 1, 0, 0, 0, 0, 0},
        // EQ1
        {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 0, 0, 0, 0},
        // EQ2
        {24, 64, 64, 75, 64, 64, 113, 64, 64, 64, 0, 0, 0, 0, 0, 0},
        // Compressor
        {-30, 2, -6, 20, 120, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        // Order
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
        // WahWah
        {64, 64, 80, 0, 0, 64, 20, 90, 0, 60, 0, 0, 0, 0, 0, 0},
        // AlienWah1
        {64, 64, 70, 0, 0, 62, 60, 105, 25, 0, 64, 0, 0, 0, 0, 0},
        // Cabinet
        {0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        // Pan
        {64, 64, 50, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
        // Harmonizer
        {64, 64, 64, 12, 64, 0, 0, 0, 64, 64, 0, 0, 0, 0, 0, 0},
        // MusicDelay
        {64, 0, 2, 7, 0, 59, 0, 127, 4, 59, 106, 75, 75, 0, 0, 0},
        // NoiseGate
        {0, 0, 1, 2, 96, 20, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        // NewDist
        {0, 64, 64, 83, 65, 15, 0, 75, 31, 68, 0, 0, 0, 0, 0, 0},
        // APhaser
        {64, 20, 40, 0, 1, 64, 110, 40, 4, 10, 0, 64, 1, 0, 0, 0},
        // Valve
        {0, 64, 64, 127, 64, 0, 5841, 61, 1, 0, 69, 1, 80, 0, 0, 0},
        // Dual Flange
        {-32, 0, 0, 110, 800, 10, -27, 16000, 1, 0, 24, 64, 1, 10, 0, 0},
        // Ring
        {-64, 0, -64, 64, 35, 1, 0, 20, 0, 40, 0, 64, 1, 0, 0, 0},
        // Exciter
        {127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20000, 20, 0, 0, 0},
        // MBDist
        {0, 64, 64, 56, 40, 0, 0, 0, 29, 35, 100, 0, 450, 1500, 1, 0},
        // Arpie
        {67, 64, 35, 64, 30, 59, 0, 127, 0, 0, 0, 0, 0, 0, 0, 0},
        // Expander
        {-50, 20, 50, 50, 3134, 76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        // Shuffle 1
        {64, 10, 0, 0, 0, 600, 1200, 2000, 6000, -14, 1, 0, 0, 0, 0, 0},
        // Synthfilter
        {0, 20, 14, 0, 1, 64, 110, -40, 6, 0, 0, 32, -32, 500, 100, 0},
        // MBVvol
        {0, 40, 0, 64, 80, 0, 0, 500, 2500, 5000, 0, 0, 0, 0, 0, 0},
        // Convolotron 1
        {67, 64, 1, 100, 0, 64, 30, 20, 0, 0, 0, 0, 0, 0, 0, 0},
        // Looper
        {64, 0, 1, 0, 1, 0, 64, 1, 0, 0, 64, 0, 0, 0, 0, 0},
        // RyanWah
        {16, 10, 60, 0, 0, 64, 0, 0, 10, 7, -16, 40, -3, 1, 2000, 450},
        // Echoverse
        {64, 64, 90, 64, 64, 64, 64, 0, 1, 64, 0, 0, 0, 0, 0, 0},
        // CoilCrafter
        {32, 6, 1, 3300, 16, 4400, 42, 20, 0, 0, 0, 0, 0, 0, 0, 0},
        // ShelfBoost
        {127, 64, 16000, 1, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        // Vocoder
        {0, 64, 10, 70, 70, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        // Sustainer
        {67, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        // Sequence
        {20, 100, 10, 50, 25, 120, 60, 127, 0, 90, 40, 0, 0, 0, 3, 0},
        // Shifter
        {0, 64, 64, 200, 200, -20, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        // StompBox
        {48, 32, 0, 32, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        // Reverbtron
        {64, 0, 1, 1500, 0, 0, 60, 18, 4, 0, 0, 64, 0, 0, 0, 0}
    };

    for (int i = 0; i < 62; i++) {
        memset(NewBank[i].Preset_Name, 0, sizeof(NewBank[i].Preset_Name));
        memset(NewBank[i].Author, 0, sizeof(NewBank[i].Author));

        NewBank[i].Input_Gain = .5f;
        NewBank[i].Master_Volume = .5f;
        NewBank[i].Balance = 1.0f;
        NewBank[i].Bypass = 0;

        memset(NewBank[i].lv, 0, sizeof(NewBank[i].lv));

        for (int j = 0; j < 42; j++) {
            for (int k = 0; k < 16; k++) {
                NewBank[i].lv[j][k] = presets[j][k];
            }
        }

        memset(NewBank[i].XUserMIDI, 0, sizeof(NewBank[i].XUserMIDI));
    }
}

int loadbank(const char* filename)
{
    int i, j, k, t;
    k = 0;
    char buf[256];
    char nfilename[256];
    FILE* fn;
    if ((fn = portable_fopen(filename, "rb")) != nullptr) {
        while (!feof(fn)) {
            if (fread(&Bank, sizeof(Bank), 1, fn) < 1) break;
        }
        fclose(fn);
        if (BigEndian()) old_fix_endianess();
        convert_IO();

        snprintf(nfilename, sizeof(nfilename), "%s.ml", filename);
        if ((fn = portable_fopen(nfilename, "r")) == nullptr)
            return 1;

        for (j = 0; j < 80; j++) {
            memset(buf, 0, sizeof(buf));
            if (!fgets(buf, sizeof buf, fn)) {break;}
            LOCAL_SSCANF(buf, "%d\n", &k);
            if (k) {
                for (i = 0; i < 128; i++) {
                    memset(buf, 0, sizeof(buf));
                    if (!fgets(buf, sizeof buf, fn)) {break;}
                    LOCAL_SSCANF(buf, "%d\n", &t);
                    if (t) {
                        memset(buf, 0, sizeof(buf));
                        if (!fgets(buf, sizeof buf, fn)) {break;}
                        LOCAL_SSCANF(buf,
                            "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                            &PML[j].XUserMIDI[i][0], &PML[j].XUserMIDI[i][1],
                            &PML[j].XUserMIDI[i][2], &PML[j].XUserMIDI[i][3],
                            &PML[j].XUserMIDI[i][4], &PML[j].XUserMIDI[i][5],
                            &PML[j].XUserMIDI[i][6], &PML[j].XUserMIDI[i][7],
                            &PML[j].XUserMIDI[i][8], &PML[j].XUserMIDI[i][9],
                            &PML[j].XUserMIDI[i][10], &PML[j].XUserMIDI[i][10],
                            &PML[j].XUserMIDI[i][12], &PML[j].XUserMIDI[i][13],
                            &PML[j].XUserMIDI[i][14], &PML[j].XUserMIDI[i][15],
                            &PML[j].XUserMIDI[i][16], &PML[j].XUserMIDI[i][17],
                            &PML[j].XUserMIDI[i][18], &PML[j].XUserMIDI[i][19]);
                    }
                }
            }
        }

        fclose(fn);
        return 1;
    }
    return 0;
}

int savebank(const char* filename)
{
    FILE* fn;
    if ((fn = portable_fopen(filename, "wb")) != nullptr) {
        copy_IO();
        if (BigEndian()) fix_endianess();
        fwrite(&NewBank, sizeof(NewBank), 1, fn);
        if (BigEndian()) fix_endianess();
        fclose(fn);
        return 1;
    }
    return 0;
}

int setdelay(int old)
{
    return 20 + lrintf((float)old / 127.0f * 1980.0f);
}

int setTempo(int old)
{
    float p_diff = 100000.0;
    float lfofreq = (powf(2.0f, (float)old / 127.0f * 10.0f) - 1.0f) * 0.03f;
    float old_incx = fabsf(lfofreq) * 256.0f / 48000.0f;

    for (int i = 1; i <= 600; i++) {
        float incx = (float)i * 256.0f / (48000.0f * 60.0f);
        if (incx == old_incx) return i;
        float diff = fabsf(incx - old_incx);
        if (diff < p_diff) p_diff = diff;
        else return i - 1;
    }
    return 1;
}

int setFreql(int old)
{
    float fr = expf(powf((float)old / 127.0f, 0.5f) * logf(25000.0f)) + 40.0f;
    return lrintf(fr);
}

int setFreqh(int old)
{
    float fr = expf(powf((float)old / 127.0f, 0.5f) * logf(25000.0f)) + 20.0f;
    return lrintf(fr);
}

int setFreq(int old)
{
    float fr = 600.0f * powf(30.0f, ((float)old - 64.0f) / 64.0f);
    return lrintf(fr);
}

void convert_bank_entries_1_60()
{
    for (int i = 1; i < 61; i++) {
        NewBank[i].Input_Gain = Bank[i].Input_Gain;
        NewBank[i].Master_Volume = Bank[i].Master_Volume;
        NewBank[i].Balance = 1.0f;
        NewBank[i].Bypass = Bank[i].Bypass;

        snprintf(NewBank[i].Preset_Name, sizeof(NewBank[i].Preset_Name), "%s", Bank[i].Preset_Name);
        snprintf(NewBank[i].Author, sizeof(NewBank[i].Author), "%s", Bank[i].Author);
        snprintf(NewBank[i].cInput_Gain, sizeof(NewBank[i].cInput_Gain), "%s", Bank[i].Reserva);
        snprintf(NewBank[i].cMaster_Volume, sizeof(NewBank[i].cMaster_Volume), "%s", Bank[i].Reserva1);
        snprintf(NewBank[i].cBalance, sizeof(NewBank[i].cBalance), "%s", "1.000000");

        for (int j = 0; j < 20; j++) {
            for (int k = 0; k < 20; k++) {
                NewBank[i].lv[j][k] = Bank[i].lv[j][k];
            }
        }

        memcpy(NewBank[i].XUserMIDI, PML[i].XUserMIDI, sizeof(NewBank[i].XUserMIDI));

        NewBank[i].lv[0][19] = Bank[i].lv[0][12];
        NewBank[i].lv[1][19] = Bank[i].lv[1][7];
        NewBank[i].lv[2][19] = Bank[i].lv[2][12];
        NewBank[i].lv[3][19] = Bank[i].lv[3][12];
        NewBank[i].lv[4][19] = Bank[i].lv[4][12];
        NewBank[i].lv[5][19] = Bank[i].lv[5][11];
        NewBank[i].lv[6][19] = Bank[i].lv[6][11];
        NewBank[i].lv[7][19] = Bank[i].lv[7][12];
        NewBank[i].lv[8][19] = Bank[i].lv[8][10];
        NewBank[i].lv[9][19] = Bank[i].lv[9][8];
        NewBank[i].lv[11][19] = Bank[i].lv[11][11];
        NewBank[i].lv[12][19] = Bank[i].lv[12][11];
        NewBank[i].lv[13][19] = Bank[i].lv[13][2];
        NewBank[i].lv[14][19] = Bank[i].lv[14][8];
        NewBank[i].lv[15][19] = Bank[i].lv[15][11];
        NewBank[i].lv[16][19] = Bank[i].lv[16][13];
        NewBank[i].lv[17][19] = Bank[i].lv[17][7];
        NewBank[i].lv[18][19] = Bank[i].lv[18][12];
        NewBank[i].lv[19][19] = Bank[i].lv[19][12];

        NewBank[i].lv[2][2] = setTempo(NewBank[i].lv[2][2]);
        NewBank[i].lv[3][2] = setTempo(NewBank[i].lv[3][2]);
        NewBank[i].lv[4][2] = setTempo(NewBank[i].lv[4][2]);
        NewBank[i].lv[11][2] = setTempo(NewBank[i].lv[11][2]);
        NewBank[i].lv[12][2] = setTempo(NewBank[i].lv[12][2]);
        NewBank[i].lv[14][2] = setTempo(NewBank[i].lv[14][2]);
        NewBank[i].lv[19][2] = setTempo(NewBank[i].lv[19][2]);

        NewBank[i].lv[1][2] = setdelay(NewBank[i].lv[1][2]);

        NewBank[i].lv[6][7] = setFreql(NewBank[i].lv[6][7]);
        NewBank[i].lv[6][8] = setFreqh(NewBank[i].lv[6][8]);
        NewBank[i].lv[5][7] = setFreql(NewBank[i].lv[5][7]);
        NewBank[i].lv[5][8] = setFreqh(NewBank[i].lv[5][8]);
        NewBank[i].lv[18][7] = setFreql(NewBank[i].lv[18][7]);
        NewBank[i].lv[18][8] = setFreqh(NewBank[i].lv[18][8]);
        NewBank[i].lv[0][7] = setFreql(NewBank[i].lv[0][7]);
        NewBank[i].lv[0][8] = setFreqh(NewBank[i].lv[0][8]);
        NewBank[i].lv[17][4] = setFreql(NewBank[i].lv[17][4]);
        NewBank[i].lv[17][5] = setFreqh(NewBank[i].lv[17][5]);
        NewBank[i].lv[15][4] = setFreq(NewBank[i].lv[15][4]);
        NewBank[i].lv[8][0] = setFreq(NewBank[i].lv[8][0]);
        NewBank[i].lv[8][3] = setFreq(NewBank[i].lv[8][3]);
        NewBank[i].lv[8][6] = setFreq(NewBank[i].lv[8][6]);
    }
}

void convert_bank_entries_61_80()
{
    for (int i = 61; i < 81; i++) {
        NewBank[i - 60].Input_Gain = Bank[i].Input_Gain;
        NewBank[i - 60].Master_Volume = Bank[i].Master_Volume;
        NewBank[i - 60].Balance = 1.0f;
        NewBank[i - 60].Bypass = Bank[i].Bypass;

        snprintf(NewBank[i - 60].Preset_Name, sizeof(NewBank[i - 60].Preset_Name), "%s", Bank[i].Preset_Name);
        snprintf(NewBank[i - 60].Author, sizeof(NewBank[i - 60].Author), "%s", Bank[i].Author);
        snprintf(NewBank[i - 60].cInput_Gain, sizeof(NewBank[i - 60].cInput_Gain), "%s", Bank[i].Reserva);
        snprintf(NewBank[i - 60].cMaster_Volume, sizeof(NewBank[i - 60].cMaster_Volume), "%s", Bank[i].Reserva1);
        snprintf(NewBank[i - 60].cBalance, sizeof(NewBank[i - 60].cBalance), "%s", "1.000000");

        for (int j = 0; j < 20; j++) {
            for (int k = 0; k < 20; k++) {
                NewBank[i - 60].lv[j][k] = Bank[i].lv[j][k];
            }
        }

        memcpy(NewBank[i - 60].XUserMIDI, PML[i].XUserMIDI,
               sizeof(NewBank[i - 60].XUserMIDI));

        NewBank[i - 60].lv[0][19] = Bank[i].lv[0][12];
        NewBank[i - 60].lv[1][19] = Bank[i].lv[1][7];
        NewBank[i - 60].lv[2][19] = Bank[i].lv[2][12];
        NewBank[i - 60].lv[3][19] = Bank[i].lv[3][12];
        NewBank[i - 60].lv[4][19] = Bank[i].lv[4][12];
        NewBank[i - 60].lv[5][19] = Bank[i].lv[5][11];
        NewBank[i - 60].lv[6][19] = Bank[i].lv[6][11];
        NewBank[i - 60].lv[7][19] = Bank[i].lv[7][12];
        NewBank[i - 60].lv[8][19] = Bank[i].lv[8][10];
        NewBank[i - 60].lv[9][19] = Bank[i].lv[9][8];
        NewBank[i - 60].lv[11][19] = Bank[i].lv[11][11];
        NewBank[i - 60].lv[12][19] = Bank[i].lv[12][11];
        NewBank[i - 60].lv[13][19] = Bank[i].lv[13][2];
        NewBank[i - 60].lv[14][19] = Bank[i].lv[14][8];
        NewBank[i - 60].lv[15][19] = Bank[i].lv[15][11];
        NewBank[i - 60].lv[16][19] = Bank[i].lv[16][13];
        NewBank[i - 60].lv[17][19] = Bank[i].lv[17][7];
        NewBank[i - 60].lv[18][19] = Bank[i].lv[18][12];
        NewBank[i - 60].lv[19][19] = Bank[i].lv[19][12];

        NewBank[i - 60].lv[2][2] = setTempo(NewBank[i - 60].lv[2][2]);
        NewBank[i - 60].lv[3][2] = setTempo(NewBank[i - 60].lv[3][2]);
        NewBank[i - 60].lv[4][2] = setTempo(NewBank[i - 60].lv[4][2]);
        NewBank[i - 60].lv[11][2] = setTempo(NewBank[i - 60].lv[11][2]);
        NewBank[i - 60].lv[12][2] = setTempo(NewBank[i - 60].lv[12][2]);
        NewBank[i - 60].lv[14][2] = setTempo(NewBank[i - 60].lv[14][2]);
        NewBank[i - 60].lv[19][2] = setTempo(NewBank[i - 60].lv[19][2]);

        NewBank[i - 60].lv[1][2] = setdelay(NewBank[i - 60].lv[1][2]);

        NewBank[i - 60].lv[6][7] = setFreql(NewBank[i - 60].lv[6][7]);
        NewBank[i - 60].lv[6][8] = setFreqh(NewBank[i - 60].lv[6][8]);
        NewBank[i - 60].lv[5][7] = setFreql(NewBank[i - 60].lv[5][7]);
        NewBank[i - 60].lv[5][8] = setFreqh(NewBank[i - 60].lv[5][8]);
        NewBank[i - 60].lv[18][7] = setFreql(NewBank[i - 60].lv[18][7]);
        NewBank[i - 60].lv[18][8] = setFreqh(NewBank[i - 60].lv[18][8]);
        NewBank[i - 60].lv[0][7] = setFreql(NewBank[i - 60].lv[0][7]);
        NewBank[i - 60].lv[0][8] = setFreqh(NewBank[i - 60].lv[0][8]);
        NewBank[i - 60].lv[17][4] = setFreql(NewBank[i - 60].lv[17][4]);
        NewBank[i - 60].lv[17][5] = setFreqh(NewBank[i - 60].lv[17][5]);
        NewBank[i - 60].lv[15][4] = setFreq(NewBank[i - 60].lv[15][4]);
        NewBank[i - 60].lv[8][0] = setFreq(NewBank[i - 60].lv[8][0]);
        NewBank[i - 60].lv[8][3] = setFreq(NewBank[i - 60].lv[8][3]);
        NewBank[i - 60].lv[8][6] = setFreq(NewBank[i - 60].lv[8][6]);
    }
}

} // anonymous namespace


int rakconvert_convert_bank(const char* old_bank_file)
{
    if (!old_bank_file) return 1;

    std::string OldBankFile(old_bank_file);

    // Load the old bank
    int loaded = loadbank(const_cast<char*>(old_bank_file));
    if (!loaded) {
        fprintf(stderr, "rakconvert: failed to load bank file: %s\n", old_bank_file);
        return 1;
    }

    printf("converting: %s\n\n", OldBankFile.c_str());

    // Generate new filenames by stripping the .rkrb extension
    if (OldBankFile.size() > 5) {
        OldBankFile.erase(OldBankFile.size() - 5, 5);
    }
    std::string NewFile1 = OldBankFile + "01_050.rkrb";
    std::string NewFile2 = OldBankFile + "02_050.rkrb";

    // Convert and save first bank (entries 1-60)
    printf("generating %s\n", NewFile1.c_str());
    New_Bank_Init();
    convert_bank_entries_1_60();
    printf("saving %s\n\n", NewFile1.c_str());
    if (!savebank(NewFile1.c_str())) {
        fprintf(stderr, "rakconvert: failed to save %s\n", NewFile1.c_str());
        return 1;
    }

    // Convert and save second bank (entries 61-80)
    printf("generating %s\n", NewFile2.c_str());
    New_Bank_Init();
    convert_bank_entries_61_80();
    printf("saving %s\n", NewFile2.c_str());
    if (!savebank(NewFile2.c_str())) {
        fprintf(stderr, "rakconvert: failed to save %s\n", NewFile2.c_str());
        return 1;
    }

    printf("Done.\n");
    return 0;
}
