/*

  Echotron.C - Echotron effect
  Author: Ryan Billing & Josep Andreu

  Adapted effect structure of ZynAddSubFX - a software synthesizer
  Author: Nasca Octavian Paul

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.

  You should have received a copy of the GNU General Public License (version 2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Echotron.h"

Echotron::Echotron (float * efxoutl_, float * efxoutr_)
{
    efxoutl = efxoutl_;
    efxoutr = efxoutr_;

    initparams=0;
    //default values
    Ppreset = 0;
    Pvolume = 50;
    Ppanning = 64;
    Plrcross = 100;
    Phidamp = 60;
    Filenum = 0;
    Plength = 10;
    Puser = 0;
    fb = 0.0f;
    lfeedback = 0.0f;
    rfeedback = 0.0f;
    subdiv_dmod = 1.0f;
    subdiv_fmod = 1.0f;
    f_qmode = 0;

    maxx_size = (SAMPLE_RATE * 6);   //6 Seconds delay time

    lxn = new delayline(6.0f, ECHOTRON_F_SIZE);
    rxn = new delayline(6.0f, ECHOTRON_F_SIZE);

    lxn->set_mix(0.0f);
    rxn->set_mix(0.0f);

    offset = 0;

    lpfl =  new AnalogFilter (0, 800, 1, 0);;
    lpfr =  new AnalogFilter (0, 800, 1, 0);;

    float center, qq;
    for (int i = 0; i < ECHOTRON_MAXFILTERS; i++) {
        center = 500;
        qq = 1.0f;
        filterbank[i].sfreq = center;
        filterbank[i].sq = qq;
        filterbank[i].sLP = 0.25f;
        filterbank[i].sBP = -1.0f;
        filterbank[i].sHP = 0.5f;
        filterbank[i].sStg = 1.0f;
        filterbank[i].l = new RBFilter (0, center, qq, 0);
        filterbank[i].r = new RBFilter (0, center, qq, 0);

        filterbank[i].l->setmix (1,filterbank[i].sLP , filterbank[i].sBP,filterbank[i].sHP);
        filterbank[i].r->setmix (1,filterbank[i].sLP , filterbank[i].sBP,filterbank[i].sHP);
    };

    setpreset (Ppreset);
    cleanup ();
};

Echotron::~Echotron ()
{
};

/*
 * Cleanup the effect
 */
void
Echotron::cleanup ()
{

    lxn->cleanup();
    rxn->cleanup();
    lxn->set_averaging(0.05f);
    rxn->set_averaging(0.05f);

    lpfl->cleanup ();
    lpfr->cleanup ();

};

/*
 * Effect output
 */
void
Echotron::out (float * smpsl, float * smpsr)
{

    int i, j, k;
    int length = Plength;
    float l,r,lyn, ryn;
    float rxindex,lxindex;


    if((Pmoddly)||(Pmodfilts)) modulate_delay();
    else interpl = interpr = 0;

    float tmpmodl = oldldmod;
    float tmpmodr = oldrdmod;

    for (i = 0; i < PERIOD; i++) {
        tmpmodl+=interpl;
        tmpmodr+=interpr;

        l = lxn->delay( (lpfl->filterout_s(smpsl[i] + lfeedback) ), 0.0f, 0, 1, 0);  //High Freq damping
        r = rxn->delay( (lpfr->filterout_s(smpsr[i] + rfeedback) ), 0.0f, 0, 1, 0);

        //Convolve
        lyn = 0.0f;
        ryn = 0.0f;

        if(Pfilters) {

            j=0;
            for (k=0; k<length; k++) {
                lxindex = ltime[k] + tmpmodl;
                rxindex = rtime[k] + tmpmodr;

                if((iStages[k]>=0)&&(j<ECHOTRON_MAXFILTERS)) {
                    lyn += filterbank[j].l->filterout_s(lxn->delay(l, lxindex, k, 0, 0)) * ldata[k];		//filter each tap specified
                    ryn += filterbank[j].r->filterout_s(rxn->delay(r, lxindex, k, 0, 0)) * rdata[k];
                    j++;
                } else {
                    lyn += lxn->delay(l, lxindex, k, 0, 0) * ldata[k];
                    ryn += rxn->delay(r, rxindex, k, 0, 0) * rdata[k];
                }

            }

        } else {
            for (k=0; k<length; k++) {
                lxindex = ltime[k] + tmpmodl;
                rxindex = rtime[k] + tmpmodr;

                lyn += lxn->delay(l, lxindex, k, 0, 0) * ldata[k];
                ryn += rxn->delay(r, rxindex, k, 0, 0) * rdata[k];
            }

        }

        lfeedback =  (lrcross*ryn + ilrcross*lyn) * lpanning;
        rfeedback = (lrcross*lyn + ilrcross*ryn) * rpanning;
        efxoutl[i] = lfeedback;
        efxoutr[i] = rfeedback;
        lfeedback *= fb;
        rfeedback *= fb;

    };

    if(initparams) init_params();

};


/*
 * Parameter control
 */
void
Echotron::setvolume (int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (float)Pvolume / 127.0f;
    if (Pvolume == 0)
        cleanup ();

};

void
Echotron::setpanning (int value)
{
    Ppanning = value;
    rpanning = ((float)Ppanning) / 64.0f;
    lpanning = 2.0f - rpanning;
    lpanning = 10.0f * powf(lpanning, 4);
    rpanning = 10.0f * powf(rpanning, 4);
    lpanning = 1.0f - 1.0f/(lpanning + 1.0f);
    rpanning = 1.0f - 1.0f/(rpanning + 1.0f);
    lpanning *= 1.1f;
    rpanning *= 1.1f;
    if(lpanning>1.0f) lpanning = 1.0f;
    if(rpanning>1.0f) rpanning = 1.0f;
};

int
Echotron::setfile(int value)
{
    float tPan=0.0f;
    float tTime=0.0f;
    float tLevel=0.0f;
    float tLP=0.0f;
    float tBP=0.0f;
    float tHP=0.0f;
    float tFreq=20.0f;
    float tQ=1.0f;
    int tiStages = 0;

    FILE *fs;

    char wbuf[128];

    if(!Puser) {
        Filenum = value;
        memset(Filename,0,sizeof(Filename));
        sprintf(Filename, "%s/%d.dly",DATADIR,Filenum+1);
    }

    if ((fs = fopen (Filename, "r")) == NULL) {
        loaddefault();
        return(0);
    }

    while (fgets(wbuf,sizeof wbuf,fs) != NULL) {
        //fgets(wbuf,sizeof wbuf,fs);
        if(wbuf[0]!='#') break;
        memset(wbuf,0,sizeof(wbuf));
    }

    sscanf(wbuf,"%f\t%f\t%d",&subdiv_fmod,&subdiv_dmod,&f_qmode); //Second line has tempo subdivision
//printf("subdivs:\t%f\t%f\n",subdiv_fmod,subdiv_dmod);

    int count = 0;
    memset(iStages,0,sizeof(iStages));



    while ((fgets(wbuf,sizeof wbuf,fs) != NULL) && (count<ECHOTRON_F_SIZE)) {
        if(wbuf[0]==10) break;  // Check Carriage Return
        if(wbuf[0]=='#') continue;
        sscanf(wbuf,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d",&tPan, &tTime, &tLevel,
               &tLP,  &tBP,  &tHP,  &tFreq,  &tQ,  &tiStages);
        //printf("params:\n%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\n",tPan, tTime, tLevel,
        //tLP,  tBP,  tHP,  tFreq,  tQ,  tiStages);
        if((tPan<-1.0f) || (tPan>1.0f)) {
            error_num=5;
            break;
        } else fPan[count]=tPan;

        if((tTime <-6.0) || (tTime>6.0f)) {
            error_num=6;
            break;
        } else fTime[count]=fabs(tTime);

        if((tLevel <-10.0f) || (tLevel>10.0f)) {
            error_num=7;
            break;
        } else fLevel[count]=tLevel;

        if((tLP <-2.0f) || (tLP>2.0f)) {
            error_num=8;
            break;
        } else fLP[count]=tLP;

        if((tBP<-2.0f) || (tBP>2.0f)) {
            error_num=9;
            break;
        } else fBP[count]=tBP;

        if((tHP<-2.0f) || (tHP>2.0f)) {
            error_num=10;
            break;
        } else fHP[count]=tHP;

        if((tFreq <20.0f) || (tFreq>26000.0f)) {
            error_num=11;
            break;
        } else fFreq[count]=tFreq;

        if((tQ <0.0) || (tQ>300.0f)) {
            error_num=12;
            break;
        } else fQ[count]=tQ;

        if((tiStages<0) || (tiStages>MAX_FILTER_STAGES)) {
            error_num=13;
            break;
        } else iStages[count]=tiStages-1;   //check in main loop if <0, then skip filter


        memset(wbuf,0,sizeof(wbuf));
        count++;
    }
    fclose(fs);

    if(!Pchange) Plength=count;
    cleanup();
    init_params();
    return(1);
};

void
Echotron::loaddefault()
{
    Plength = 1;
    fPan[0] = 0.0f;  //
    fTime[0] = 1.0f;  //default 1 measure delay
    fLevel[0] = 0.7f;
    fLP[0] = 1.0f;
    fBP[0] = -1.0f;
    fHP[0] = 1.0f;
    fFreq[0]=800.0f;;
    fQ[0]=2.0f;
    iStages[0]=1;
    subdiv_dmod = 1.0f;
    subdiv_fmod = 1.0f;
    init_params();
}

void Echotron::init_params()
{

    float hSR = fSAMPLE_RATE*0.5f;
    float tpanl, tpanr;
    float tmptempo;
    int tfcnt = 0;

    initparams=0;
    depth = ((float) (Pdepth - 64))/64.0f;
    dlyrange = 0.008*f_pow2(4.5f*depth);
    width = ((float) Pwidth)/127.0f;

    tmptempo = (float) Ptempo;
    lfo.Pfreq = lrintf(subdiv_fmod*tmptempo);
    dlfo.Pfreq = lrintf(subdiv_dmod*tmptempo);

    for(int i=0; i<Plength; i++) {
// tmp_time=lrintf(fTime[i]*tempo_coeff*fSAMPLE_RATE);
// if(tmp_time<maxx_size) rtime[i]=tmp_time; else rtime[i]=maxx_size;

        ltime[i] = rtime[i] = fTime[i]*tempo_coeff;


        if(fPan[i]>=0.0f) {
            tpanr = 1.0;
            tpanl = 1.0f - fPan[i];
        } else {
            tpanl = 1.0;
            tpanr = 1.0f + fPan[i];
        }

        ldata[i]=fLevel[i]*tpanl;
        rdata[i]=fLevel[i]*tpanr;

        if((tfcnt<ECHOTRON_MAXFILTERS)&&(iStages[i]>=0)) {
            int Freq=fFreq[i]*f_pow2(depth*4.5f);
            if (Freq<20.0) Freq=20.0f;
            if (Freq>hSR) Freq=hSR;
            filterbank[tfcnt].l->setfreq_and_q(Freq,fQ[i]);
            filterbank[tfcnt].r->setfreq_and_q(Freq,fQ[i]);
            filterbank[tfcnt].l->setstages(iStages[i]);
            filterbank[tfcnt].r->setstages(iStages[i]);
            filterbank[tfcnt].l->setmix (1, fLP[i] , fBP[i], fHP[i]);
            filterbank[tfcnt].r->setmix (1, fLP[i] , fBP[i], fHP[i]);
            filterbank[tfcnt].l->setmode(f_qmode);
            filterbank[tfcnt].r->setmode(f_qmode);
            tfcnt++;
        }
    }

};

void Echotron::modulate_delay()
{

    float lfmod, rfmod, lfol, lfor, dlfol, dlfor;
    float fperiod = 1.0f/fPERIOD;

    lfo.effectlfoout (&lfol, &lfor);
    dlfo.effectlfoout (&dlfol, &dlfor);
    if(Pmodfilts) {
        lfmod = f_pow2((lfol*width + 0.25f + depth)*4.5f);
        rfmod = f_pow2((lfor*width + 0.25f + depth)*4.5f);
        for(int i=0; i<ECHOTRON_MAXFILTERS; i++) {

            filterbank[i].l->setfreq(lfmod*fFreq[i]);
            filterbank[i].r->setfreq(rfmod*fFreq[i]);

        }

    }

    if(Pmoddly) {
        oldldmod = ldmod;
        oldrdmod = rdmod;
        ldmod = width*dlfol;
        rdmod = width*dlfor;

// ldmod=lrintf(dlyrange*tempo_coeff*fSAMPLE_RATE*ldmod);
// rdmod=lrintf(dlyrange*tempo_coeff*fSAMPLE_RATE*rdmod);
        ldmod=dlyrange*tempo_coeff*ldmod;
        rdmod=dlyrange*tempo_coeff*rdmod;
        interpl = (ldmod - oldldmod)*fperiod;
        interpr = (rdmod - oldrdmod)*fperiod;
    } else {
        oldldmod = 0.0f;
        oldrdmod = 0.0f;
        ldmod = 0.0f;
        rdmod = 0.0f;
        interpl = 0.0f;
        interpr = 0.0f;
    }

};


void
Echotron::sethidamp (int Phidamp)
{
    this->Phidamp = Phidamp;
    hidamp = 1.0f - (float)Phidamp / 127.1f;
    float fr = 20.0f*f_pow2(hidamp*10.0f);
    lpfl->setfreq (fr);
    lpfr->setfreq (fr);
};

void
Echotron::setfb(int value)
{

    fb = (float)value/64.0f;
}


void
Echotron::setpreset (int npreset)
{
    const int PRESET_SIZE = 16;
    const int NUM_PRESETS = 5;
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Summer
        {64, 45, 34, 4, 0, 76, 3, 41, 0, 96, -13, 64, 1, 1, 1, 1},
        //Ambience
        {96, 64, 16, 4, 0, 180, 50, 64, 1, 96, -4, 64, 1, 0, 0, 0},
        //Arranjer
        {64, 64, 10, 4, 0, 400, 32, 64, 1, 96, -8, 64, 1, 0, 0, 0},
        //Suction
        {0, 47, 28, 8, 0, 92, 0, 64, 3, 32, 0, 64, 1, 1, 1, 1},
        //SucFlange
        {64, 36, 93, 8, 0, 81, 0, 64, 3, 32, 0, 64, 1, 0, 1, 1}

    };

    if(npreset>NUM_PRESETS-1) {
        Fpre->ReadPreset(41,npreset-NUM_PRESETS+1);
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, pdata[n]);
    } else {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar (n, presets[npreset][n]);
    }
    Ppreset = npreset;
};


void
Echotron::changepar (int npar, int value)
{
    float tmptempo;
    switch (npar) {
    case 0:
        setvolume (value);
        break;
    case 1:
        Pdepth=value;
        initparams=1;
        break;
    case 2:
        Pwidth=value;
        initparams=1;
        break;
    case 3:
        Plength = value;
        if(Plength>127) Plength = 127;
        initparams=1;
        break;
    case 4:
        Puser = value;
        break;
    case 5:
        Ptempo = value;

        tmptempo = (float) Ptempo;
        tempo_coeff = 60.0f / tmptempo;
        lfo.Pfreq = lrintf(subdiv_fmod*tmptempo);
        dlfo.Pfreq = lrintf(subdiv_dmod*tmptempo);
        lfo.updateparams ();
        initparams=1;
        break;
    case 6:
        sethidamp (value);
        break;
    case 7:
        Plrcross = value;
        lrcross = ((float)(Plrcross)-64)/64.0;
        ilrcross = 1.0f - abs(lrcross);
        break;
    case 8:
        if(!setfile(value)) error_num=4;
        break;
    case 9:
        lfo.Pstereo = value;
        dlfo.Pstereo = value;
        lfo.updateparams ();
        dlfo.updateparams ();
        break;
    case 10:
        Pfb = value;
        setfb(value);
        break;
    case 11:
        setpanning (value);
        break;
    case 12:
        Pmoddly = value;//delay modulation on/off
        break;
    case 13:
        Pmodfilts = value;//filter modulation on/off
        if(!Pmodfilts) initparams=1;
        break;
    case 14:
        //LFO Type
        lfo.PLFOtype = value;
        lfo.updateparams ();
        dlfo.PLFOtype = value;
        dlfo.updateparams ();
        break;
    case 15:
        Pfilters = value;//Pfilters
        break;
    };
};

int
Echotron::getpar (int npar)
{
    switch (npar) {
    case 0:
        return (Pvolume);
        break;
    case 1:
        return (Pdepth);
        break;
    case 2:
        return(Pwidth);
        break;
    case 3:
        return(Plength);
        break;
    case 8:
        return (Filenum);
        break;
    case 5:
        return (Ptempo);
        break;
    case 6:
        return (Phidamp);
        break;
    case 7:
        return(Plrcross);
        break;
    case 4:
        return(Puser);
        break;
    case 9:
        return(lfo.Pstereo);
        break;
    case 10:
        return(Pfb);
        break;
    case 11:
        return(Ppanning);
        break;
    case 12:
        return(Pmoddly);  //modulate delays
        break;
    case 13:
        return(Pmodfilts); //modulate filters
        break;
    case 14:
        return(lfo.PLFOtype);
        break;
    case 15:
        return(Pfilters);  //Filter delay line on/off
        break;


    };
    return (0);			//in case of bogus parameter number
};
