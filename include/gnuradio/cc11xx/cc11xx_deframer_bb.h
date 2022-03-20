/* -*- c++ -*- */
/*
 * Copyright 2014 Andre Puschmann <andre@puschmann.net>.
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


#ifndef INCLUDED_CC11XX_CC11XX_DEFRAMER_BB_H
#define INCLUDED_CC11XX_CC11XX_DEFRAMER_BB_H

#include <gnuradio/cc11xx/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace cc11xx {

    /*!
     * \brief CC11XX deframer takes in unpacked bits, and outputs PDU
     * binary blobs. This is intended for use with the
     * correlate_access_code_tag_bb block, which should have an access code
     * of "01010101010101010101010101010101" for use with CC11XX frames.
     * Frames which do not pass CRC are rejected.
     * \ingroup cc11xx
     *
     */
    class CC11XX_API cc11xx_deframer_bb : virtual public gr::block
    {
     public:
      typedef std::shared_ptr<cc11xx_deframer_bb> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of cc11xx::cc11xx_deframer_bb.
       *
       * To avoid accidental use of raw pointers, cc11xx::cc11xx_deframer_bb's
       * constructor is in a private implementation
       * class. cc11xx::cc11xx_deframer_bb::make is the public interface for
       * creating new instances.
       */
      static sptr make(const uint8_t& sync1 = 0xaa, const uint8_t& sync2 = 0xbb, const bool has_whitening = true, const bool has_crc = true, const int preamble_bytes = 4);
    };

  } // namespace cc11xx
} // namespace gr

#endif /* INCLUDED_CC11XX_CC11XX_DEFRAMER_BB_H */

