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

#ifndef INCLUDED_RPITX_RPITX_SOURCE_IMPL_H
#define INCLUDED_RPITX_RPITX_SOURCE_IMPL_H

#include <rpitx/rpitx_source.h>
#include <librpitx/iqdmasync.h>

namespace gr {
  namespace rpitx {

    class rpitx_source_impl : public rpitx_source
    {
     private:
      iqdmasync *iqtest;
      // Nothing to declare in this block.

     public:
      rpitx_source_impl(float samp_rate, float carrier_freq);
      ~rpitx_source_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace rpitx
} // namespace gr

#endif /* INCLUDED_RPITX_RPITX_SOURCE_IMPL_H */

