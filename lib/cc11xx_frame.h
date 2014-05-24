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

#ifndef CC11XX_FRAME_H
#define CC11XX_FRAME_H

#include <stdint.h>
#include <iostream>
#include <boost/format.hpp>
#include "cc11xx_utility.h"

namespace gr {
namespace cc11xx {

class cc11xx_frame
{
public:
    cc11xx_frame(bool hasWhitening, bool hasCrc);
    ~cc11xx_frame() {};

    void add_byte(const uint8_t byte);
    int get_missing_bytes(void);
    int get_total_length(void);
    int get_payload_length(void);
    bool is_complete(void);
    bool verify_crc(void);
    void print_frame(void);
    void reset();
    std::vector<uint8_t> get_payload(void);

private:
    int payload_length_;
    int frame_length_;
    bool hasWhitening_;
    bool hasCrc_;
    std::vector<uint8_t> buffer_;
};

}
}

#endif // CC11XX_FRAME_H
