/*
  ZynAddSubFX - a software synthesizer

  Looper.h - Looper Effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Ryan Billing & Josep Andreu
  Reverse Echo by Transmogrifox

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

#ifndef LOOPER_H
#define LOOPER_H

#include "dsp_constants.hpp"
#include "metronome.hpp"
#include "Effect.hpp"

class Looper : public Effect
{
public:
    Looper (float size);
    ~Looper ();
    void out (float * smpsl, float * smpr) override;
    void out (float * smpsl, float * smpr, int nframes) override;
    void setMaxBlockSize (int maxBlockSize) override;
    void setpreset (int npreset);
    void loadpreset (int npar, int value);  // to set one from a preset
    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanuppt1 ();
    void cleanuppt2 ();
    void cleanup ();
    void settempo(int value);
    void setmvol(int value);
    int looper_bar;
    int looper_qua;
    int progstate[6]{};

    void getstate ();



    int Pplay;	//set to 1
    int Pstop;	//set to 1


private:

    void initdelays ();
    void setfade ();
    void setbar(int value);
    void timeposition(int value);
    int set_len(int value);
    int cal_len(int value);



    //Parameters
    int Pvolume;	//Mix
    int Precord;	//set to 1
    int Pclear;   //set to 1
    int Preverse;	//0 or 1
    int Pfade1;    //0...127//  crossfade track1 & 2
    int Pfade2;
    int PT1;
    int PT2;
    int Pautoplay;
    int Prec1;
    int Prec2;
    int Plink;
    int Ptempo;
    int Pbar;
    int Pmetro;
    int Pms;

    // Loop positions and lengths. The constructor never called initdelays(),
    // so these reached out() holding whatever was on the heap; kl2 in
    // particular is a direct subscript into t2ldelay/t2rdelay.
    int kl{0}, kl2{0}, rvkl{0}, rvkl2{0};
    int maxx_delay{0}, fade{0}, dl{1}, dl2{1};
    int first_time1{1}, first_time2{1}, rplaystate{0};
    int barlen{0}, looper_ts{0};

    std::vector<float> ldelay, rdelay;
    std::vector<float> t2ldelay, t2rdelay;

    float oldl{0.0f}, oldr{0.0f};		//pt. lpf

    float  Srate_Attack_Coeff{0.0f}, track1gain{0.0f}, track2gain{0.0f};
    float  fade1{0.0f}, fade2{0.0f}, pregain1{0.0f}, pregain2{0.0f};
    float mvol{0.0f};
    class metronome ticker;
    std::vector<float> ticktock{};
};


#endif
