/* -*- c++ -*- */
/*
 * Copyright 2014 Andre Puschmann <andre.puschmann@tu-ilmenau.de>.
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

#ifndef INCLUDED_CC11XX_CC11XX_DEFRAMER_BB_IMPL_H
#define INCLUDED_CC11XX_CC11XX_DEFRAMER_BB_IMPL_H

#include <cc11xx/cc11xx_deframer_bb.h>
#include "cc11xx_utility.h"
#include "cc11xx_frame.h"

namespace gr {
  namespace cc11xx {

    class cc11xx_deframer_bb_impl : public cc11xx_deframer_bb
    {
     private:
        enum {STATE_PREAMBLE_SEARCH, STATE_SYNC_SEARCH, STATE_HAVE_SYNC, STATE_HAVE_HEADER} d_state;

        std::vector<uint8_t> d_sync;
        int d_preamble_bytes;
        cc11xx_frame d_frame;
        int d_frames_good;
        int d_frames_bad;

        // private members
        void enter_preamble_search();
        void enter_sync_search();
        void enter_have_sync();
        uint8_t readByteFromInput(const uint8_t* in, int offset = 0);

     public:
      cc11xx_deframer_bb_impl(const uint8_t& sync1, const uint8_t& sync2, const bool has_whitening, const bool has_crc, const int preamble_bytes);
      ~cc11xx_deframer_bb_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
               gr_vector_int &ninput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
    };

  } // namespace cc11xx
} // namespace gr

#endif /* INCLUDED_CC11XX_CC11XX_DEFRAMER_BB_IMPL_H */

