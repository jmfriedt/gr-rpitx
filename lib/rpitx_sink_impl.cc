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
#include "rpitx_sink_impl.h"

#include <unistd.h>
#include <librpitx/librpitx.h>
#include "stdio.h"
#include <cstring>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t th;

bool  running=true;
bool  fdds=false;            //operate as a DDS
float drivedds=0.1;          //drive level

#define IQSize 4096

namespace gr {
  namespace rpitx {

    rpitx_sink::sptr
    rpitx_sink::make(float samp_rate, float carrier_freq)
    {
      return gnuradio::get_initial_sptr
        (new rpitx_sink_impl(samp_rate, carrier_freq));
    }

    /*
     * The private constructor
     */
    rpitx_sink_impl::rpitx_sink_impl(float samp_rate, float carrier_freq)
      : gr::sync_block("rpitx_sink",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(0, 0, 0))
    {
	iqtest=new iqdmasync(carrier_freq,samp_rate,14,IQSize*4,MODE_IQ);
	iqtest->SetPLLMasterLoop(3,4,0);
        samp_rate_=samp_rate;
        pthread_mutex_init(&th, NULL);
	printf("Welcome to gr-rpitx\n");
    }

    /*
     * Our virtual destructor.
     */
    rpitx_sink_impl::~rpitx_sink_impl()
    {iqtest->stop();
     delete(iqtest);
     pthread_mutex_destroy(&th);
    }

    void rpitx_sink_impl::set_freq(float freq)
    { printf("new frequency: %f\n",freq);
      pthread_mutex_lock(&th);
      iqtest->stop();
      delete(iqtest);
      iqtest=new iqdmasync(freq,samp_rate_,14,IQSize*4,MODE_IQ);
      iqtest->SetPLLMasterLoop(3,4,0);
      pthread_mutex_unlock(&th);
    }
 
    int
    rpitx_sink_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      std::complex<float> CIQBuffer[IQSize];	
      int k;
      int Harmonic=1;
      const gr_complex *in = (const gr_complex *) input_items[0];
      int nbread=0,xferlen;

      int CplxSampleNumber=0;
      while (nbread<noutput_items)
        {if (nbread+IQSize<noutput_items) xferlen=IQSize; else xferlen=noutput_items-nbread;
         pthread_mutex_lock(&th);
         iqtest->SetIQSamples((std::complex<float>*)&in[nbread],xferlen,Harmonic);
         pthread_mutex_unlock(&th);
         nbread+=xferlen;
        }
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
  } /* namespace rpitx */
} /* namespace gr */
