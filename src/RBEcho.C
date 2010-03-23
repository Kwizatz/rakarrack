/*
  ZynAddSubFX - a software synthesizer
 
  RBEcho.C - Echo effect
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  Modified for rakarrack by Josep Andreu
  Reverse Echo effect by Transmogrifox
  
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
#include "RBEcho.h"

RBEcho::RBEcho (float * efxoutl_, float * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  //default values
  Ppreset = 0;
  Pvolume = 50;
  Ppanning = 64;
  Pdelay = 60;
  Plrdelay = 100;
  Plrcross = 100;
  Pfb = 40;
  Phidamp = 60;

  ldelay = NULL;
  rdelay = NULL;
  lrdelay = 0;
  Srate_Attack_Coeff = 1.0f / (fSAMPLE_RATE * ATTACK);
  maxx_delay = SAMPLE_RATE * MAX_DELAY;

  ldelay = new float[maxx_delay];  
  rdelay = new float[maxx_delay];
  
  setpreset (Ppreset);
  cleanup ();
};

RBEcho::~RBEcho ()
{
  delete[]ldelay;
  delete[]rdelay;
};

/*
 * Cleanup the effect
 */
void
RBEcho::cleanup ()
{
  int i;
  for (i = 0; i < maxx_delay; i++)
    ldelay[i] = 0.0;
  for (i = 0; i < maxx_delay; i++)
    rdelay[i] = 0.0;
  oldl = 0.0;
  oldr = 0.0;
};


/*
 * Initialize the delays
 */
void
RBEcho::initdelays ()
{
  kl = 0;
  kr = 0;

  dl = delay - lrdelay;
  if (dl < 1)
    dl = 1;
  dr = delay + lrdelay;
  if (dr < 1)
    dr = 1;

  rvkl = dl - 1;
  rvkr = dr - 1;
  Srate_Attack_Coeff = 15.0f * cSAMPLE_RATE;   // Set swell time to 66ms of average delay time 


  cleanup ();
};

/*
 * Effect output
 */
void
RBEcho::out (float * smpsl, float * smpsr)
{
  int i;
  float l, r, ldl, rdl, rswell, lswell;
  

  for (i = 0; i < PERIOD; i++)
    {
      ldl = ldelay[kl];
      rdl = rdelay[kr];
      l = ldl * (1.0f - lrcross) + rdl * lrcross;
      r = rdl * (1.0f - lrcross) + ldl * lrcross;
      ldl = l;
      rdl = r;

      ldl = smpsl[i] * panning - ldl * fb;
      rdl = smpsr[i] * (1.0f - panning) - rdl * fb;

      
      
      //LowPass Filter
      ldelay[kl] = ldl = ldl * hidamp + oldl * (1.0f - hidamp);
      rdelay[kr] = rdl = rdl * hidamp + oldr * (1.0f - hidamp);
      oldl = ldl + DENORMAL_GUARD;
      oldr = rdl + DENORMAL_GUARD;

      
      if (++kl >= dl)
	kl = 0;
      if (++kr >= dr)
	kr = 0;
      rvkl = dl - 1 - kl;
      rvkr = dr - 1 - kr;

          
      if(reverse > 0.0f)
      {

      lswell =	(float)(abs(kl - rvkl)) * Srate_Attack_Coeff;
	      if (lswell <= PI) 
	      {
	      lswell = (1.0f - cosf(lswell));  //Clickless transition
	      efxoutl[i] = reverse * (ldelay[rvkl] * lswell)  + (ldelay[kl] * (1-reverse));   //Volume ducking near zero crossing.     
	      }  
	      else
	      {
	      efxoutl[i] = 2.0f * ((ldelay[rvkl] * reverse)  + (ldelay[kl] * (1-reverse)));        
	      }
       
      rswell = 	(float)(abs(kr - rvkr)) * Srate_Attack_Coeff;  
	      if (rswell <= PI)
	      {
	       rswell = (1.0f - cosf(rswell));   //Clickless transition 
	       efxoutr[i] = reverse * (rdelay[rvkr] * rswell)  + (rdelay[kr] * (1-reverse));  //Volume ducking near zero crossing.
	      }
	      else
	      {
	      efxoutr[i] = 2.0f * ((rdelay[rvkr] * reverse)  + (rdelay[kr] * (1-reverse)));
	      }
      

      }
      else
      {
      efxoutl[i]= 2.0f * ldelay[kl];
      efxoutr[i]= 2.0f * rdelay[kr];
      }      


            
    };

};


/*
 * Parameter control
 */
void
RBEcho::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;
  volume = outvolume = (float)Pvolume / 127.0f;
  if (Pvolume == 0)
    cleanup ();

};

void
RBEcho::setpanning (int Ppanning)
{
  this->Ppanning = Ppanning;
  panning = ((float)Ppanning + 0.5f) / 127.0f;
};

void
RBEcho::setreverse (int Preverse)
{
  this->Preverse = Preverse;
  reverse = (float) Preverse / 127.0f;
};

void
RBEcho::setdelay (int Pdelay)
{
  this->Pdelay = Pdelay;
  delay=Pdelay;
  if (delay < 10) delay = 10;
  if (delay > MAX_DELAY * 1000) delay = 1000 * MAX_DELAY;  //Constrains 10ms ... MAX_DELAY
  delay = 1 + lrintf ( ((float) delay / 1000.0f) * fSAMPLE_RATE );	

  initdelays ();
};

void
RBEcho::setlrdelay (int Plrdelay)
{
  float tmp;
  this->Plrdelay = Plrdelay;
  tmp =
    (powf (2.0, fabsf ((float)Plrdelay - 64.0f) / 64.0f * 9.0f) -
     1.0f) / 1000.0f * fSAMPLE_RATE;
  if (Plrdelay < 64.0)
    tmp = -tmp;
  lrdelay = lrintf(tmp);
  initdelays ();
};

void
RBEcho::setlrcross (int Plrcross)
{
  this->Plrcross = Plrcross;
  lrcross = (float)Plrcross / 127.0f * 1.0f;
};

void
RBEcho::setfb (int Pfb)
{
  this->Pfb = Pfb;
  fb = (float)Pfb / 128.0f;
};

void
RBEcho::sethidamp (int Phidamp)
{
  this->Phidamp = Phidamp;
  hidamp = 1.0f - (float)Phidamp / 127.0f;
};

void
RBEcho::setpreset (int npreset)
{
  const int PRESET_SIZE = 8;
  const int NUM_PRESETS = 9;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Echo 1
    {67, 64, 565, 64, 30, 59, 0, 127},
    //Echo 2
    {67, 64, 357, 64, 30, 59, 0, 64},
    //Echo 3
    {67, 75, 955, 64, 30, 59, 10, 0},
    //Simple Echo
    {67, 60, 705, 64, 30, 0, 0, 0},
    //Canyon
    {67, 60, 1610, 50, 30, 82, 48, 0},
    //Panning Echo 1
    {67, 64, 705, 17, 0, 82, 24, 0},
    //Panning Echo 2
    {81, 60, 737, 118, 100, 68, 18, 0},
    //Panning Echo 3
    {81, 60, 472, 100, 127, 67, 36, 0},
    //Feedback Echo
    {62, 64, 456, 64, 100, 90, 55, 0}
  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
};


void
RBEcho::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
    case 1:
      setpanning (value);
      break;
    case 2:
      setdelay (value);
      break;
    case 3:
      setlrdelay (value);
      break;
    case 4:
      setlrcross (value);
      break;
    case 5:
      setfb (value);
      break;
    case 6:
      sethidamp (value);
      break;
    case 7:
      setreverse (value);
      break;

    };
};

int
RBEcho::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (Ppanning);
      break;
    case 2:
      return (Pdelay);
      break;
    case 3:
      return (Plrdelay);
      break;
    case 4:
      return (Plrcross);
      break;
    case 5:
      return (Pfb);
      break;
    case 6:
      return (Phidamp);
      break;
    case 7:
      return (Preverse);
      break;

    };
  return (0);			//in case of bogus parameter number
};