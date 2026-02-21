/*
  rakarrack - a guitar effects software

  StereoHarm.C  -  Stereo Harmonizer
  Copyright (C) 2008 Josep Andreu
  Author:  Josep Andreu

  Using Stephan M. Bernsee smbPitchShifter engine.

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/


#include "StereoHarm.hpp"
#include <cstring>
#include "FPreset.hpp"



StereoHarm::StereoHarm (float *efxoutl_, float *efxoutr_, long int Quality, int DS, int uq, int dq)
{



    efxoutl = efxoutl_;
    efxoutr = efxoutr_;
    hq = Quality;
    adjust(DS);

    templ.resize(PERIOD);
    tempr.resize(PERIOD);


    outil.resize(nPERIOD);
    outir.resize(nPERIOD);

    outol.resize(nPERIOD);
    outor.resize(nPERIOD);

    U_Resample = std::make_unique<Resample>(dq);
    D_Resample = std::make_unique<Resample>(uq);


    chromel=0.0;
    chromer=0.0;


    PSl = std::make_unique<PitchShifter> (window, hq, nfSAMPLE_RATE);
    PSl->ratio = 1.0f;
    PSr = std::make_unique<PitchShifter> (window, hq, nfSAMPLE_RATE);
    PSr->ratio = 1.0f;

    Ppreset = 0;
    PMIDI = 0;
    mira = 0;
    setpreset (Ppreset);


    cleanup ();

};



StereoHarm::~StereoHarm () = default;

void
StereoHarm::cleanup ()
{
    mira = 0;
    chromel=0;
    chromer=0;
    memset(outil.data(), 0, sizeof(float)*nPERIOD);
    memset(outir.data(), 0, sizeof(float)*nPERIOD);
    memset(outol.data(), 0, sizeof(float)*nPERIOD);
    memset(outor.data(), 0, sizeof(float)*nPERIOD);

};


void
StereoHarm::out (float *smpsl, float *smpsr)
{

    int i;


    if(DS_state != 0) {
        memcpy(templ.data(), smpsl,sizeof(float)*PERIOD);
        memcpy(tempr.data(), smpsr,sizeof(float)*PERIOD);
        U_Resample->out(templ.data(),tempr.data(),smpsl,smpsr,PERIOD,u_up);
    }


    for (i = 0; i < nPERIOD; i++) {


        outil[i] = smpsl[i];
        if (outil[i] > 1.0)
            outil[i] = 1.0f;
        if (outil[i] < -1.0)
            outil[i] = -1.0f;

        outir[i] = smpsr[i];
        if (outir[i] > 1.0)
            outir[i] = 1.0f;
        if (outir[i] < -1.0)
            outir[i] = -1.0f;

    }

    if ((PMIDI) || (PSELECT)) {
        PSl->ratio = r__ratio[1];
        PSr->ratio = r__ratio[2];
    }

    if (PSl->ratio != 1.0f) {
        PSl->smbPitchShift (PSl->ratio, nPERIOD, window, hq, nfSAMPLE_RATE, outil.data(), outol.data());
    } else
        memcpy(outol.data(),outil.data(),sizeof(float)*nPERIOD);


    if (PSr->ratio != 1.0f) {
        PSr->smbPitchShift (PSr->ratio, nPERIOD, window, hq, nfSAMPLE_RATE, outir.data(), outor.data());
    } else
        memcpy(outor.data(),outir.data(),sizeof(float)*nPERIOD);


    if(DS_state != 0) {
        D_Resample->out(outol.data(),outor.data(),templ.data(),tempr.data(),nPERIOD,u_down);
    } else {
        memcpy(templ.data(), outol.data(),sizeof(float)*PERIOD);
        memcpy(tempr.data(), outor.data(),sizeof(float)*PERIOD);

    }


    for (i = 0; i < PERIOD; i++) {
        efxoutl[i] = (templ[i] * (1.0f - lrcross) + tempr[i] * lrcross)* gainl;
        efxoutr[i] = (tempr[i] * (1.0f - lrcross) + templ[i] * lrcross)* gainr;
    }



};



void
StereoHarm::setvolume (int value)
{
    this->Pvolume = value;
    outvolume = (float)Pvolume / 127.0f;
};



void
StereoHarm::setgain (int chan, int value)
{

    switch(chan) {
    case 0:
        Pgainl = value;
        gainl = (float)Pgainl / 127.0f;
        gainl *=2.0;
        break;
    case 1:
        Pgainr = value;
        gainr = (float)Pgainr / 127.0f;
        gainr *=2.0;
        break;

    }
};


void
StereoHarm::setinterval (int chan, int value)
{

    switch(chan) {
    case 0:
        Pintervall = value;
        intervall = (float)Pintervall - 12.0f;
        PSl->ratio = powf(2.0f,intervall / 12.0f)+chromel;
        if (Pintervall % 12 == 0)
            mira = 0;
        else
            mira = 1;
        break;

    case 1:
        Pintervalr = value;
        intervalr = (float)Pintervalr - 12.0f;
        PSr->ratio = powf(2.0f,intervalr / 12.0f)+chromer;
        if (Pintervalr % 12 == 0)
            mira = 0;
        else
            mira = 1;
        break;
    }
};



void
StereoHarm::setchrome (int chan, int value)
{

    float max,min;
    max = 0.0;
    min = 0.0;

    switch(chan) {
    case 0:
        max = powf(2.0f,(intervall+1.0f) / 12.0f);
        min = powf(2.0f,(intervall-1.0f) / 12.0f);
        break;
    case 1:
        max = powf(2.0f,(intervalr+1.0f) / 12.0f);
        min = powf(2.0f,(intervalr-1.0f) / 12.0f);
        break;
    }

    if (max > 2.0) max=2.0f;
    if (min < 0.5) min=0.5f;


    switch(chan) {
    case 0:
        Pchromel=value;
        chromel=(max-min)/4000.0f*(float)value;
        PSl->ratio=powf(2.0f,intervall/12.0f)+chromel;
        break;
    case 1:
        Pchromer=value;
        chromer=(max-min)/4000.0f*(float)value;
        PSr->ratio=powf(2.0f,intervalr/12.0f)+chromer;
        break;
    }

}

void
StereoHarm::setMIDI (int value)
{

    this->PMIDI = value;
}


void
StereoHarm::adjust(int DS)
{

    DS_state=DS;


    switch(DS) {

    case 0:
        nPERIOD = PERIOD;
        nSAMPLE_RATE = SAMPLE_RATE;
        nfSAMPLE_RATE = fSAMPLE_RATE;
        window = 2048;
        break;

    case 1:
        nPERIOD = lrintf(fPERIOD*96000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 96000;
        nfSAMPLE_RATE = 96000.0f;
        window = 2048;
        break;


    case 2:
        nPERIOD = lrintf(fPERIOD*48000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 48000;
        nfSAMPLE_RATE = 48000.0f;
        window = 2048;
        break;

    case 3:
        nPERIOD = lrintf(fPERIOD*44100.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 44100;
        nfSAMPLE_RATE = 44100.0f;
        window = 2048;
        break;

    case 4:
        nPERIOD = lrintf(fPERIOD*32000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 32000;
        nfSAMPLE_RATE = 32000.0f;
        window = 2048;
        break;

    case 5:
        nPERIOD = lrintf(fPERIOD*22050.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 22050;
        nfSAMPLE_RATE = 22050.0f;
        window = 1024;
        break;

    case 6:
        nPERIOD = lrintf(fPERIOD*16000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 16000;
        nfSAMPLE_RATE = 16000.0f;
        window = 1024;
        break;

    case 7:
        nPERIOD = lrintf(fPERIOD*12000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 12000;
        nfSAMPLE_RATE = 12000.0f;
        window = 512;
        break;

    case 8:
        nPERIOD = lrintf(fPERIOD*8000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 8000;
        nfSAMPLE_RATE = 8000.0f;
        window = 512;
        break;

    case 9:
        nPERIOD = lrintf(fPERIOD*4000.0f/fSAMPLE_RATE);
        nSAMPLE_RATE = 4000;
        nfSAMPLE_RATE = 4000.0f;
        window = 256;
        break;
    }



    u_up= (double)nPERIOD / (double)PERIOD;
    u_down= (double)PERIOD / (double)nPERIOD;
}


void
StereoHarm::setlrcross (int value)
{
    Plrcross = value;
    lrcross = (float)Plrcross / 127.0f;

};



void
StereoHarm::setpreset (int npreset)
{
    const int PRESET_SIZE = 12;
    const int NUM_PRESETS = 4;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Plain
        {64, 64, 12, 0, 64, 12, 0, 0, 0, 0, 0, 64},
        //Octavador
        {64, 64, 0, 0, 64, 0, 0, 0, 0, 0, 0, 64},
        //Chorus
        {64, 64, 12, 80, 64, 12, -80, 0, 0, 0, 0, 64},
        //Chorus
        {64, 64, 12, 280, 64, 12, -280, 0, 0, 0, 0, 64}

    };

    cleanup();
    if(npreset>NUM_PRESETS-1) {
        FPreset::ReadPreset(42,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }

    Ppreset = npreset;


};



void
StereoHarm::changepar (int npar, int value)
{

    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        setgain (0,value);
        break;
    case 2:
        setinterval(0,value);
        break;
    case 3:
        setchrome(0, value);
        break;
    case 4:
        setgain (1,value);
        break;
    case 5:
        setinterval(1,value);
        break;
    case 6:
        setchrome(1, value);
        break;
    case 7:
        PSELECT = value;;
        break;
    case 8:
        Pnote = value;
        break;
    case 9:
        Ptype = value;
        if (Ptype==0) {
            setchrome(0, Pchromel);
            setchrome(1, Pchromer);
        }
        break;
    case 10:
        setMIDI (value);
        break;
    case 11:
        setlrcross(value);
        break;


    }


};


int
StereoHarm::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Pgainl);
        break;
    case 2:
        return (Pintervall);
        break;
    case 3:
        return (Pchromel);
        break;
    case 4:
        return (Pgainr);
        break;
    case 5:
        return (Pintervalr);
        break;
    case 6:
        return (Pchromer);
        break;
    case 7:
        return (PSELECT);
        break;
    case 8:
        return (Pnote);
        break;
    case 9:
        return (Ptype);
        break;
    case 10:
        return (PMIDI);
        break;
    case 11:
        return(Plrcross);
        break;
    default:
        return (0);

    }

};
