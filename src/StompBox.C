/*
  Rakarrack   Audio FX software 
  Stompbox.C - stompbox modeler
  Modified for rakarrack by Josep Andreu
  
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
#include "StompBox.h"



StompBox::StompBox (float * efxoutl_, float * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;


  //default values
  Ppreset = 0;
  Pvolume = 50;
  
  //left channel filters
  linput = new AnalogFilter (1, 80.0f, 1.0f, 0);  //  AnalogFilter (unsigned char Ftype, float Ffreq, float Fq,unsigned char Fstages);
  lpre1 = new AnalogFilter (1, 630.0f, 1.0f, 0);   // LPF = 0, HPF = 1
  lpre2 = new AnalogFilter (1, 220.0f, 1.0f, 0);
  lpost = new AnalogFilter (0, 1000.0f, 1.0f, 0);
  ltonehg = new AnalogFilter (1, 2500.0f, 1.0f, 0);  
  ltonemd = new AnalogFilter (1, 700.0f, 1.0f, 0);  
  ltonelw = new AnalogFilter (0, 150.0f, 1.0, 0); 
  
  //Right channel filters
  linput = new AnalogFilter (1, 80.0f, 1.0f, 0);  //  AnalogFilter (unsigned char Ftype, float Ffreq, float Fq,unsigned char Fstages);
  lpre1 = new AnalogFilter (1, 630.0f, 1.0f, 0);   // LPF = 0, HPF = 1
  lpre2 = new AnalogFilter (1, 220.0f, 1.0f, 0);
  lpost = new AnalogFilter (0, 1000.0f, 1.0f, 0);
  ltonehg = new AnalogFilter (1, 2500.0f, 1.0f, 0);  
  ltonemd = new AnalogFilter (1, 700.0f, 1.0f, 0);  
  ltonelw = new AnalogFilter (0, 150.0f, 1.0, 0); 
   
  cleanup ();

  setpreset (Ppreset);
};

StompBox::~StompBox ()
{


};

/*
 * Cleanup the effect
 */
void
StompBox::cleanup ()
{

 
};


/*
 * Effect output
 */
void
StompBox::out (float * smpsl, float * smpsr)
{
int i;

   switch (Pmode)
   {
    case 1:


    for(i=0; i<PERIOD; i++)
      {

       }

    break; 






   } 



};




/*
 * Parameter control
 */
void
StompBox::setvolume (int value)
{
  Pvolume = value;
  outvolume = (float)Pvolume / 127.0f;

};

void
StompBox::setpreset (int npreset)
{
  const int PRESET_SIZE = 5;
  const int NUM_PRESETS = 1;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Trebble
    {0, 0, 0, 0, 0}
  
  };


  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
  cleanup ();

};


void
StompBox::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
     case 1:
      Phigh = value;
      break;
     case 2: 
      Pmid = value;
      break;
    case 3:
      Plow = value;
      break;
    case 4:
      Pgain = value;
      gain = 24.0f * (float)value/64.0f;
      break;
    case 5:
      Pmode = value;
      break;
 
    };
};

int
StompBox::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (Pgain);
      break;
    case 2:
      return (Phigh);
      break;
    case 3:
      return (Pmid);
      break;
    case 4:
      return (Plow);
      break; 
    case 5:
      return (Pmode);
      break; 

    };
  return (0);			//in case of bogus parameter number
};
