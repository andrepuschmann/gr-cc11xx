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

/* Documentation:
 * http://www.ti.com/lit/an/swra097/swra097.pdf
 *
 *
 * CRC implementation:
 * http://www.ti.com/lit/an/swra111d/swra111d.pdf
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "cc11xx_deframer_bb_impl.h"

#include <errno.h>
#include <iostream>
#include <cstdio>

#include <boost/format.hpp>

#define VERBOSE 0
#define VERBOSE_PREAMBLE_SEARCH 0
#define VERBOSE_SYNC_SEARCH 0
#define VERBOSE_HAVE_SYNC 0
#define VERBOSE_HAVE_HEADER 0

namespace gr {
namespace cc11xx {

cc11xx_deframer_bb::sptr
cc11xx_deframer_bb::make(const uint8_t& sync1, const uint8_t& sync2, const bool has_whitening, const bool has_crc, const int preamble_bytes)
{
    return gnuradio::get_initial_sptr
            (new cc11xx_deframer_bb_impl(sync1, sync2, has_whitening, has_crc, preamble_bytes));
}

/*
   * The private constructor
   */
cc11xx_deframer_bb_impl::cc11xx_deframer_bb_impl(const uint8_t& sync1, const uint8_t& sync2, const bool has_whitening, const bool has_crc, const int preamble_bytes)
    : gr::block("cc1100_framedecoder_bb",
                gr::io_signature::make(1, 1, sizeof(uint8_t)),
                gr::io_signature::make(0, 0, 0))
    ,d_frame(has_whitening, has_crc)
    ,d_preamble_bytes(preamble_bytes)
    ,d_frames_good(0)
    ,d_frames_bad(0)
{
    assert(preamble_bytes == 2 || preamble_bytes == 4);

    // assemble sync word
    for (int i = 0; i < preamble_bytes; i+=2) {
        d_sync.push_back(sync1);
        d_sync.push_back(sync2);
    }

    d_state = STATE_PREAMBLE_SEARCH;
    message_port_register_out(pmt::mp("out"));
}


/*
 * Our virtual destructor.
 */
cc11xx_deframer_bb_impl::~cc11xx_deframer_bb_impl()
{
    std::cout << boost::format("Good frames: %d\n") % (d_frames_good);
    std::cout << boost::format("Bad frames: %d\n") % (d_frames_bad);
}


void
cc11xx_deframer_bb_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
    /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
    ninput_items_required[0] = noutput_items;
}


void cc11xx_deframer_bb_impl::enter_preamble_search()
{
    if (VERBOSE_PREAMBLE_SEARCH)
        std::cout << boost::format("@ enter_preamble_search()\n");

    d_state = STATE_PREAMBLE_SEARCH;
}


void cc11xx_deframer_bb_impl::enter_sync_search()
{
    if (VERBOSE_SYNC_SEARCH)
        std::cout << boost::format("@ enter_sync_search()\n");

    d_state = STATE_SYNC_SEARCH;
}


void cc11xx_deframer_bb_impl::enter_have_sync()
{
    if (VERBOSE_HAVE_SYNC)
        std::cout << boost::format("@ enter_have_sync()\n");

    d_state = STATE_HAVE_SYNC;
    d_offset = 0;
    d_frame.reset();
}


int
cc11xx_deframer_bb_impl::general_work (int noutput_items,
                                       gr_vector_int &ninput_items,
                                       gr_vector_const_void_star &input_items,
                                       gr_vector_void_star &output_items)
{
    const uint8_t* in = (const uint8_t*)input_items[0];
    int ninput = ninput_items[0];
    uint64_t abs_ninput = nitems_read(0);

    if (VERBOSE) {
        std::cout << boost::format("@ general_work()\n");
        std::cout << boost::format("absolut ninput %d\n") % abs_ninput;
        std::cout << boost::format("ninput %d\n") % ninput;
    }

    switch(d_state) {
    case STATE_PREAMBLE_SEARCH:
        {
            if (VERBOSE_PREAMBLE_SEARCH) {
                std::cout << boost::format("PREAMBLE_SEARCH: ninput=%d\n") % ninput;
            }

            // set offset to end of input buffer
            // if a tag is present, it will be set to its position
            d_offset = ninput;

            // read tags from input data
            std::vector<tag_t> tags;
            get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + ninput);
            for (size_t i = 0; i < tags.size(); i++) {
                if (VERBOSE_PREAMBLE_SEARCH)
                    std::cout << boost::format("Found tag with key: %s\n") % pmt::symbol_to_string(tags[i].key);
                if (pmt::symbol_to_string(tags[i].key) == "preamble") {
                    // update offset to this tag
                    d_offset = tags[i].offset - abs_ninput;
                    assert(d_offset <= ninput);
                    if (VERBOSE_PREAMBLE_SEARCH)
                        std::cout << boost::format("Preamble found at %d\n") % d_offset;

                    enter_sync_search();
                    break;
                }
            }

            consume(0, d_offset);
        }
        break;

    case STATE_SYNC_SEARCH:
        {
            if (VERBOSE_SYNC_SEARCH) {
                std::cout << boost::format("SYNC_SEARCH: ninput=%d\n") % ninput;
            }

            int bits_needed = d_preamble_bytes * 8;
            int consumed_items = 0;
            if (ninput >= bits_needed) {
                // read sync bytes
                bool sync_found = true;
                for (int i = 0; i < d_preamble_bytes; i++) {
                    uint8_t tmp = cc11xx_utility::read_byte_from_input(in, i * 8);
                    if (tmp != d_sync[i]) {
                        sync_found = false;
                        break;
                    }
                }

                if (sync_found) {
                    std::cout << boost::format("SYNC FOUND!\n");
                    consumed_items = d_preamble_bytes * 8;
                    enter_have_sync();
                } else {
                    // sync bytes not found, make sure to consume on item to get the next tag
                    consumed_items = 1;
                    enter_preamble_search();
                }
            }
            consume(0, consumed_items);
        }
        break;

    case STATE_HAVE_SYNC:
        {
            // skip if not enough items
            if (ninput < 8) {
                consume(0, 0);
                break;
            }
            assert(ninput >= 8);

            // read the frame length
            d_frame.add_byte(cc11xx_utility::read_byte_from_input(in));

            if (VERBOSE_HAVE_SYNC) {
                std::cout << boost::format("HAVE_SYNC:\n");
                std::cout << boost::format("Frame length: %d\n") % (d_frame.get_total_length());
                std::cout << boost::format("Payload length: %d\n") % (d_frame.get_payload_length());
            }

            // transit to next state immedietly
            d_state = STATE_HAVE_HEADER;
            consume(0, 8);
        }
        break;

    case STATE_HAVE_HEADER:
        {
            if (VERBOSE_HAVE_HEADER)
                std::cout << boost::format("HAVE_HEADER:\n");

            // determine available bytes in this call
            int available_bytes = ceil(ninput / 8);
            int needed_bytes = d_frame.get_missing_bytes();
            int consumable_bytes = std::min(available_bytes, needed_bytes);

            if (consumable_bytes < 1) {
                consume(0, 0);
                break;
            }
            assert(consumable_bytes > 0);

            if (VERBOSE_HAVE_HEADER)
                std::cout << boost::format("Reading next %d payload byte(s) of total %d.\n") % consumable_bytes % d_frame.get_total_length();
            for (int i = 0; i < consumable_bytes; i++) {
                uint8_t byte = cc11xx_utility::read_byte_from_input(in, i * 8);
                d_frame.add_byte(byte);
            }

            if (d_frame.is_complete()) {
                if (d_frame.verify_crc()) {
                    if (VERBOSE_HAVE_HEADER)
                        std::cout << boost::format("Frame received correctly!\n");

                    // publish message
                    std::vector<uint8_t> payload = d_frame.get_payload();
                    pmt::pmt_t pdu(pmt::cons(pmt::PMT_NIL, pmt::make_blob(&payload[0], payload.size())));
                    message_port_pub(pmt::mp("out"), pdu);

                    d_frames_good++;
                } else {
                    // count CRC error and drop frame
                    d_frames_bad++;
                }
                d_frame.print_frame();

                // re-enter preamble search
                d_state = STATE_PREAMBLE_SEARCH;
            }
            consume(0, consumable_bytes * 8);
        }
        break;
    default:
        assert(0);
        break;
    }

    if (VERBOSE) {
        std::cout << boost::format("Samples Processed: %d!\n") % ninput;
    }

    // Tell runtime system how many output items we produced.
    return 0;
}

} /* namespace cc11xx */
} /* namespace gr */

