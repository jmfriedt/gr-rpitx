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

bool  running=true;
bool  fdds=false;            //operate as a DDS
float drivedds=0.1;          //drive level

#define IQSize 4096

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
	iqtest=new iqdmasync(carrier_freq,samp_rate,14,IQSize*4,MODE_IQ); // 2 = typeiq_float in enum
	iqtest->SetPLLMasterLoop(3,4,0);
    }

    /*
     * Our virtual destructor.
     */
    rpitx_source_impl::~rpitx_source_impl()
    {
     iqtest->stop();
     delete(iqtest);
    }

    int
    rpitx_source_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      std::complex<float> CIQBuffer[IQSize];	
      int k;
      int Harmonic=1;
      const gr_complex *in = (const gr_complex *) input_items[0];
      int nbread=0,xferlen;

      printf("noutput_items=%d\n",noutput_items);
      // Do <+signal processing+>
      int CplxSampleNumber=0;
      while (nbread<noutput_items)
        {if (nbread+IQSize<noutput_items) xferlen=IQSize; else xferlen=noutput_items-nbread;
         iqtest->SetIQSamples((std::complex<float>*)&in[nbread],xferlen,Harmonic);
         nbread+=xferlen;
        }
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }
  } /* namespace rpitx */
} /* namespace gr */
