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

#include "cc11xx_frame.h"

namespace gr {
namespace cc11xx {

cc11xx_frame::cc11xx_frame(bool hasWhitening, bool hasCrc) :
    hasWhitening_(hasWhitening),
    hasCrc_(hasCrc)
{
    reset();
}


void cc11xx_frame::print_frame(void)
{
    std::cout << "---" << std::endl;
    std::cout << boost::format("Frame length (Bytes): %d\n") % frame_length_;
    std::cout << boost::format("Payload length (Bytes): %d\n") % payload_length_;

    std::vector<uint8_t>::const_iterator it = buffer_.begin();
    for (it = buffer_.begin(); it != buffer_.end(); ++it) {
        // int cast is neccessary, std::hex doesn't know uint8_t
        std::cout << boost::format("%02X ") % (static_cast<int>(*it));
    }
    std::cout << std::endl << "---" << std::endl;
}


void cc11xx_frame::add_byte(const uint8_t byte)
{
    uint8_t tmp = byte;
    if (hasWhitening_) {
        tmp ^= pn9_table[buffer_.size()];
    }

    // first byte is length byte
    if (buffer_.empty()) {
        payload_length_ = tmp;
        frame_length_ = payload_length_ + 1; // we need to count length byte as part of frame too
        if (hasCrc_)
            frame_length_ += 2;
    }

    // finally, add byte to buffer
    buffer_.push_back(tmp);
}


bool cc11xx_frame::is_complete()
{
    // buffer_index must match payload length
    return (frame_length_ == buffer_.size());
}


bool cc11xx_frame::verify_crc()
{
    if (not hasCrc_) return true;

    // calculate CRC
    uint16_t crc = cc11xx_utility::calc_crc(&buffer_[0], payload_length_ + 1);

    // get CRC as received from origin
    uint16_t rx_crc = buffer_[buffer_.size() - 2];
    rx_crc = (rx_crc << 8) | buffer_[buffer_.size() - 1];

    if (rx_crc != crc) {
        std::cout << boost::format("CRC mismatch: Received: 0x%04x Calculated: 0x%04x\n")
                     % (static_cast<int>(rx_crc))
                     % (static_cast<int>(crc));
        return false;
    }
    return true;
}


std::vector<uint8_t> cc11xx_frame::get_payload(void)
{
    // return byte vector constructed from internal frame buffer
    return (std::vector<uint8_t>(buffer_.begin() + 1, buffer_.begin() + 1 + payload_length_));
}


int cc11xx_frame::get_missing_bytes()
{
    return frame_length_ - buffer_.size();
}


int cc11xx_frame::get_total_length()
{
    return frame_length_;
}


int cc11xx_frame::get_payload_length()
{
    return payload_length_;
}


void cc11xx_frame::reset()
{
    payload_length_ = 0;
    frame_length_ = 0;
    buffer_.clear();
}

}
}
