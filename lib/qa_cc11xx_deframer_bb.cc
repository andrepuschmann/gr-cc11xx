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


#include <gnuradio/attributes.h>
#include <cppunit/TestAssert.h>
#include "qa_cc11xx_deframer_bb.h"
#include <cc11xx/cc11xx_deframer_bb.h>
#include <cc11xx_frame.h>
#include "cc11xx_utility.h"

namespace gr {
  namespace cc11xx {

    void
    qa_cc11xx_deframer_bb::t1_crc()
    {
        uint8_t rx_bytes[] = { 0x08, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0x5a, 0xb6};
        cc11xx_frame frame(false, true); // frame with CRC but no whitening
        for (int i = 0; i < sizeof(rx_bytes); i++) {
            frame.add_byte(rx_bytes[i]);
        }

        CPPUNIT_ASSERT(frame.is_complete() == true);
        CPPUNIT_ASSERT(frame.verify_crc() == true);
    }

    void
    qa_cc11xx_deframer_bb::t2_whitening()
    {
        uint8_t rx_bytes[] = { 0xf5, 0xe1, 0x1c };
        const uint8_t data[] = { 0x0A, 0x00, 0x01 };

        // apply de-whitening
        int ret = cc11xx_utility::xor_pn9(&rx_bytes[0], sizeof(rx_bytes));
        for (int i = 0; i < sizeof(rx_bytes); i++ ) {
            CPPUNIT_ASSERT(rx_bytes[i] == data[i]);
        }
    }

  } /* namespace cc11xx */
} /* namespace gr */

