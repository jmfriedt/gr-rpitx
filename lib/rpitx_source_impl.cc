/* -*- c++ -*- */
/*
 * Copyright 2021 JMF.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "rpitx_source_impl.h"

#include <unistd.h>
#include <librpitx/librpitx.h>
#include "stdio.h"
#include <cstring>
#include <signal.h>
#include <stdlib.h>
#include <sys/shm.h>		//Used for shared memory
// =============================================================================================
// ----- SHARED MEMORY STRUCTURE -----
// All commands sent by a partner program towards sendiq
// Operation tips
//      1-Partner program creates a shared memory block with a given arbitrary token id
//        (any integer non-zero integer number).
//      2-sendiq is started with the -m {token} argument, without it no shared memory commands
//        are honored.
//      3-Partner program willing to alter the operation of sendiq place the
//        appropriate commnand plus optional associated data and/or common_data
//        if needed (see table below).
//      4-Partner program set updated=true.
//      5-sendiq checks periodically for condition updated==true.
//      6-if detected updated==true at step -5- the command is evaluated and executed.
//      7-sendiq set updated=false.
//      8-Partner program must refrain to send any command while the condition updated==true is on
//
//      Table
//      ---------------------------------------------------------------------------
//      Command       Meaning                  data                    common_data
//       1111         Switch to I/Q mode       N/A                     N/A
//       2222         Switch to Freq & A mode  N/A                     N/A
//       3333         Set drive level          Drive level {0..7}      N/A
//       4444         Change Frequency         Frequency in Hz         N/A
//      ---------------------------------------------------------------------------
//
// Note: with current level if shared memory is enabled the data incoming thru standar input
// or file is expected to be in float format. Other data formats aren't supported together with
// commands, for them sendiq will work but no commands will be honored.
//
// *--- Memory structure


bool  running=true;
bool  fdds=false;            //operate as a DDS
float drivedds=0.1;          //drive level


namespace gr {
  namespace rpitx {

    rpitx_source::sptr
    rpitx_source::make(float samp_rate, float carrier_freq)
    {
      return gnuradio::get_initial_sptr
        (new rpitx_source_impl(samp_rate, carrier_freq));
    }


    /*
     * The private constructor
     */
    rpitx_source_impl::rpitx_source_impl(float samp_rate, float carrier_freq)
      : gr::sync_block("rpitx_source",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(0, 0, 0))
    {
//	MODE_IQ=typeiq_float;
        int FifoSize=32768;
	iqtest=new iqdmasync(carrier_freq,samp_rate,14,FifoSize,2); // 2 = typeiq_float in enum
	iqtest->SetPLLMasterLoop(3,4,0);
printf("Constructor completed\n");
    }

    /*
     * Our virtual destructor.
     */
    rpitx_source_impl::~rpitx_source_impl()
    {delete(iqtest);
    }

    int
    rpitx_source_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      int Harmonic=1;
      float SetFrequency=434e6/5;
      float SampleRate=48000;
      const gr_complex *in = (const gr_complex *) input_items[0];

printf("%d\n",noutput_items);
      if (noutput_items>32768) noutput_items=32768; // JMF: should never happen !
      // Do <+signal processing+>
      iqtest->SetIQSamples((std::complex<float>*)in,noutput_items,Harmonic);
      //iqtest->SetIQSamples(in,noutput_items,Harmonic);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
  } /* namespace rpitx */
} /* namespace gr */

/*
void print_usage(void)
{

fprintf(stderr,\
"\nsendiq -%s\n\
Usage:\nsendiq [-i File Input][-s Samplerate][-l] [-f Frequency] [-h Harmonic number] \n\
-i            path to File Input \n\
-s            SampleRate 10000-250000 \n\
-f float      central frequency Hz(50 kHz to 1500 MHz),\n\
-m int        shared memory token,\n\
-d            dds mode,\n\
-p            power level (0.00 to 7.00),\n\
-l            loop mode for file input\n\
-h            Use harmonic number n\n\
-t            IQ type (i16 default) {i16,u8,float,double}\n\
-?            help (this help).\n\
\n",\
PROGRAM_VERSION);

} 


#define MAX_SAMPLERATE 200000

int main(int argc, char* argv[])
{
	int a;
	int anyargs = 1;
	bool loop_mode_flag=false;
	enum {typeiq_i16,typeiq_u8,typeiq_float,typeiq_double};
	int InputType=typeiq_i16;
	int Decimation=1;
        
	while(1)
	{
		case 'd': // dds mode
			fdds=true;
			break;
		case 'p': // driver level (power)
			drivedds=atof(optarg);
			if (drivedds<0.0) {drivedds=0.0;}
			if (drivedds>7.0) {drivedds=7.0;}
			break;
		case 'f': // Frequency
			SetFrequency = atof(optarg);
			break;
                case 'm': // Shared memory token
                        sharedmem_token = atoi(optarg);
     	                InputType=typeiq_float;      //if using shared memory force float pipe
			break;
		case 's': // SampleRate (Only needeed in IQ mode)
			SampleRate = atoi(optarg);
			if(SampleRate>MAX_SAMPLERATE) 
			{
				for(int i=2;i<12;i++) //Max 10 times samplerate
				{
					if(SampleRate/i<MAX_SAMPLERATE) 
					{
						SampleRate=SampleRate/i;
						Decimation=i;
						break;
					}
				}
				if(Decimation==1)
				{
					 fprintf(stderr,"SampleRate too high : >%d sample/s",10*MAX_SAMPLERATE);
					 exit(1);
				} 
				else
				{
					fprintf(stderr,"Warning samplerate too high, decimation by %d will be performed",Decimation);	 
				}
			};
			break;
		case 'h': // help
			Harmonic=atoi(optarg);
			break;
		case 'l': // loop mode
			loop_mode_flag = true;
			break;
		case 't': // inout type
			if(strcmp(optarg,"i16")==0) InputType=typeiq_i16;
			if(strcmp(optarg,"u8")==0) InputType=typeiq_u8;
			if(strcmp(optarg,"float")==0) InputType=typeiq_float;
			if(strcmp(optarg,"double")==0) InputType=typeiq_double;
			break;
		case -1:
        	break;
		case '?':
			if (isprint(optopt) )
 			{
 				fprintf(stderr, "sendiq: unknown option `-%c'.\n", optopt);
 			}
			else
			{
				fprintf(stderr, "sendiq: unknown option character `\\x%x'.\n", optopt);
			}
			print_usage();

			exit(1);
			break;			
		default:
			print_usage();
			exit(1);
			break;
		}
	}

	 for (int i = 0; i < 64; i++) {
        struct sigaction sa;

        std::memset(&sa, 0, sizeof(sa));
        sa.sa_handler = terminate;
        sigaction(i, &sa, NULL);
    }

	#define IQBURST 4000
	int SR=48000;
	int FifoSize=IQBURST*4;




       sharedmem->data=0;
   }

	std::complex<float> CIQBuffer[IQBURST];	
	while(running)
	{
		
			int CplxSampleNumber=0;
			switch(InputType)
			{
				case typeiq_i16:
				{
					static short IQBuffer[IQBURST*2];
					int nbread=fread(IQBuffer,sizeof(short),IQBURST*2,iqfile);
					for(int i=0;i<nbread/2;i++)
						{
							if(i%Decimation==0)
							{		
								CIQBuffer[CplxSampleNumber++]=std::complex<float>(IQBuffer[i*2]/32768.0,IQBuffer[i*2+1]/32768.0); 
							}
						}
				}
			}
				break;
				case typeiq_u8:
				{
					static unsigned char IQBuffer[IQBURST*2];
					int nbread=fread(IQBuffer,sizeof(unsigned char),IQBURST*2,iqfile);
					for(int i=0;i<nbread/2;i++)
					{
						CIQBuffer[CplxSampleNumber++]=std::complex<float>((IQBuffer[i*2]-127.5)/128.0,(IQBuffer[i*2+1]-127.5)/128.0);
					}
				}
				break;
				case typeiq_float:
				{
					static float IQBuffer[IQBURST*2];
					int nbread=fread(IQBuffer,sizeof(float),IQBURST*2,iqfile);
					for(int i=0;i<nbread/2;i++)
						{
								CIQBuffer[CplxSampleNumber++]=std::complex<float>(IQBuffer[i*2],IQBuffer[i*2+1]);
							}
						}
					}
				}
				break;	
				case typeiq_double:
				{
					static double IQBuffer[IQBURST*2];
					int nbread=fread(IQBuffer,sizeof(double),IQBURST*2,iqfile);
					{
						for(int i=0;i<nbread/2;i++)
						{
							if(i%Decimation==0)
							{	
								CIQBuffer[CplxSampleNumber++]=std::complex<float>(IQBuffer[i*2],IQBuffer[i*2+1]);
										
							}		 
						}
					}
				}
				break;	
			
		}
	}
}	

*/
