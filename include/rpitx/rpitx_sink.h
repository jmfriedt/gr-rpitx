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

#ifndef INCLUDED_RPITX_RPITX_SOURCE_H
#define INCLUDED_RPITX_RPITX_SOURCE_H

#include <rpitx/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace rpitx {

    /*!
     * \brief <+description of block+>
     * \ingroup rpitx
     *
     */
    class RPITX_API rpitx_sink : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<rpitx_sink> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of rpitx::rpitx_sink.
       *
       * To avoid accidental use of raw pointers, rpitx::rpitx_sink's
       * constructor is in a private implementation
       * class. rpitx::rpitx_sink::make is the public interface for
       * creating new instances.
       */
	static sptr make(float samp_rate, float carrier_freq);
	virtual void set_freq(float)=0;      
    };

  } // namespace rpitx
} // namespace gr

#endif /* INCLUDED_RPITX_RPITX_SOURCE_H */

