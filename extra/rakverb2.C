

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sndfile.h>
#include <getopt.h>


#define PERIOD 128

 
 

void
show_help ()
{
  fprintf (stderr, "Usage: rakverb2 -i [Input File] [-o] [Output File]\n\n" );
  fprintf (stderr,
	   "  -h ,     --help \t\t\t display command-line help and exit\n");
  fprintf (stderr,
	   "  -i ,     --input \t\t\t Input File\n");
  fprintf (stderr,
	   "  -o ,     --output \t\t\t Output File\n");
  fprintf (stderr,
	   "  -l ,     --length \t\t\t Reflections\n");
  fprintf (stderr,
	   "  -t ,     --time \t\t\t Time Difference\n");
	   
  fprintf (stderr, "\n");

}




 

int
main(int argc, char *argv[])
{
 int option_index = 0, opt;
 int exitwithhelp = 0;
 char wbuf[2048];
 int i;
 int step=1;
 int readcount;
 int have_output=0;
 int des_len=1500;
 FILE *fn;
 SNDFILE *infile = NULL;
 SF_INFO sfinfo;
 char Outputfile[128];
 char Inputfile[128];
 char tempfile[128];
 float *buf;
 float *index, *data;
 float sample;
 float testzero, time, tmp, iSR, lastbuf;
 float compress;
 float incr;
 float time_dif=0.0;
 int x = 0;
 int ok = 0;
 int all_ok=0;
  fprintf (stderr,
   "\nrackverb2 convert Reverb IR wav files to the rakarrack file format.\nrackverb - Copyright (c) Josep Andreu - Ryan Billing \n\n");


  struct option opts[] = {
    {"length", 1, NULL, 'l'},
    {"time", 1, NULL, 't'},
    {"input", 1, NULL, 'i'},
    {"output", 1, NULL, 'o'},
    {"help", 0, NULL, 'h'},
    {0, 0, 0, 0}
  };

  while (1)
    {
      opt = getopt_long (argc, argv, "l:t:i:o:h", opts, &option_index);
      char *optarguments = optarg;

      if (opt == -1)
	break;
      switch (opt)
	{
	case 'h':
	  exitwithhelp = 1;
	  break;
        case 't':
          if (optarguments != NULL) time_dif=atof(optarguments); else time_dif=0.0;
          break;
	case 'l':
          if (optarguments != NULL) des_len=atoi(optarguments); else des_len=1500;
          break;
       	case 'i':
        if (optarguments != NULL)
	    {
	    strcpy(Inputfile,optarguments); 
            if(!(infile = sf_open(Inputfile, SFM_READ, &sfinfo))) return(0);
            all_ok=1;
            }
          break;  
        case 'o':
           if (optarguments != NULL)
	    {
	    have_output = 1;
            strcpy(Outputfile,optarguments);
            }
            break;
         }

    }

  if (exitwithhelp != 0)
    {
      show_help ();
      return (0);
    };

  if ((argc < 3) || (!all_ok))
{
    fprintf (stderr, "Try 'rakverb2 --help' for usage options.\n");
    return(0);
}    


 buf   =  (float*) malloc (sizeof (float) * PERIOD*sfinfo.channels);
 index =  (float*) malloc (sizeof (float) * sfinfo.frames*sfinfo.channels); // put the max size
 data  =  (float*) malloc (sizeof (float) * sfinfo.frames*sfinfo.channels); // put the max size


if(!have_output)
{
 strncpy(tempfile, Inputfile, 128);
 tempfile[strlen(tempfile)-4] = 0;
 sprintf(Outputfile, "%s.rvb",tempfile);
}

// open output and save headers

  if ((fn = fopen (Outputfile, "w")) == NULL)  return(0);
  memset (wbuf, 0, sizeof (wbuf));
  sprintf(wbuf,"%s\n",Inputfile);    
  fputs (wbuf, fn);

incr = 0.00001f;
while(ok == 0)
{

// read file
readcount = sf_seek (infile,0, SEEK_SET);
readcount = 1;
time = 0.0f;
tmp = 0.0f;
lastbuf = 0.0f;
iSR = 1.0f/((float) sfinfo.samplerate);
if (sfinfo.channels==1) step = 1;
if (sfinfo.channels==2) step = 2;

while( readcount > 0)
{ 
readcount = sf_readf_float(infile,buf,PERIOD);  //PERIOD is defined UP ... change if its short.


//
//process the data here 
//

for (i = 0; i<(PERIOD*sfinfo.channels); i+=step) {

     if(step==1) sample= buf[i];
     if(step==2) sample= (buf[i]+buf[i+1])*.5;

     tmp += sample;
     testzero = sample * lastbuf;       //one-liner zero crossing detection 
     if(testzero < 0.0f) {
     if(x==0)
     {
       data[x] = tmp;
       index[x] = time;
       x++;
     }
     else
     { 
       if(time - index[x-1] <time_dif) data[x-1]+=tmp*(1.0f -(time-index[x-1]));
       else
       if (((data[x-1]>0) && (tmp<0)) || ((data[x-1]<0) && (tmp>0)))
       {
       if ((fabsf(tmp-data[x-1]) > incr) && (time - index[x-1] >=time_dif))
        {
         data[x] = tmp;
         index[x] = time;
         x++;
        }
      }
     }    
     tmp = 0.0f;
     }	
     time+=iSR;
     lastbuf = sample;
}
//index data in index[x]
//data data in data[x]

}

printf("%d ",x);
if(x < des_len) ok=1; else x = 0;
incr +=0.0001f;
 

}


compress = 100.0f * ((float) x)/((float) sfinfo.frames);
printf("\nCompression: %3.2f%% ", compress);
printf("Increment : %f\n", incr);
sf_close(infile);



memset(wbuf,0,sizeof(wbuf));
sprintf(wbuf,"%f,%f\n", compress, incr);
fputs(wbuf,fn);

memset(wbuf,0,sizeof(wbuf));
sprintf(wbuf,"%d\n", x);
fputs(wbuf,fn);

for(i=0;i<x;i++)
{ 
  memset(wbuf,0,sizeof(wbuf));
  sprintf(wbuf, "%f,%f\n",index[i],data[i]);
  fputs(wbuf,fn);
};


fclose(fn);
free(buf);
free(index);
free(data);
return(0);
}


