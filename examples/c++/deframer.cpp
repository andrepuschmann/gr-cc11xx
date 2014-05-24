/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <gnuradio/top_block.h>
#include <gnuradio/audio/sink.h>
#include <gnuradio/blocks/vector_source_b.h>
#include <gnuradio/blocks/vector_to_stream.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/file_source.h>
#include <gnuradio/digital/correlate_access_code_tag_bb.h>
#include "cc11xx/cc11xx_deframer_bb.h"

using namespace gr;

int main(int argc, char **argv)
{
  std::string preamble = "10101010101010101010101010101010";
  unsigned char sync1 = 0xd3;
  unsigned char sync2 = 0x91;
  // create some source data
  // this is the sample frame from page 6 (http://www.ti.com/lit/an/swra097/swra097.pdf)
  static const unsigned char bits[] = { 1, 0, 1, 1, 1, 1, 0, 0,  // 0xbc 3 random bytes
                                        1, 1, 1, 0, 0, 1, 0, 1,  // 0xe5
                                        0, 0, 1, 0, 0, 0, 1, 0,  // 0x22
                                        1, 0, 1, 0, 1, 0, 1, 0,  // 0xaa 4 byte preamble
                                        1, 0, 1, 0, 1, 0, 1, 0,  // 0xaa
                                        1, 0, 1, 0, 1, 0, 1, 0,  // 0xaa
                                        1, 0, 1, 0, 1, 0, 1, 0,  // 0xaa
                                        1, 1, 0, 1, 0 ,0 ,1, 1,  // 0xd3 4 byte repeated sync word
                                        1, 0, 0, 1, 0, 0, 0, 1,  // 0x91
                                        1, 1, 0, 1, 0 ,0 ,1, 1,  // 0xd3
                                        1, 0, 0, 1, 0, 0, 0, 1,  // 0x91
                                        0, 0, 0, 0, 1, 0, 0, 0,  // 0x08 1 byte frame length
                                        1, 1, 1, 1, 0, 0, 0, 0,  // 0xf0 8 bytes payload
                                        1, 1, 1, 1, 0, 0, 0, 0,  // 0xf0
                                        1, 1, 1, 1, 0, 0, 0, 0,  // 0xf0
                                        1, 1, 1, 1, 0, 0, 0, 0,  // 0xf0
                                        1, 1, 1, 1, 0, 0, 0, 0,  // 0xf0
                                        1, 1, 1, 1, 0, 0, 0, 0,  // 0xf0
                                        1, 1, 1, 1, 0, 0, 0, 0,  // 0xf0
                                        1, 1, 1, 1, 0, 0, 0, 0,  // 0xf0
                                        0, 1, 0, 1, 1, 0, 1, 0,  // 0x5a 2 bytes CRC
                                        1, 0, 1, 1, 0, 1, 1, 0,  // 0xb6
                                        1, 1, 0, 1, 1, 1, 0, 1}; // 0xdd

  std::vector<unsigned char> data(bits, bits + sizeof(bits) / sizeof(bits[0]));

  // Construct a top block that will contain flowgraph blocks.  Alternatively,
  // one may create a derived class from top_block and hold instantiated blocks
  // as member data for later manipulation.
  top_block_sptr tb = make_top_block("cc11xx_deframer");

  // create a vector source
  blocks::vector_source_b::sptr src = blocks::vector_source_b::make(data);

  // create a vector to stream converter
  blocks::vector_to_stream::sptr conv = blocks::vector_to_stream::make(1, 1);

  // file source reading from /dev/urandom
  //blocks::file_source::sptr src = blocks::file_source::make(1, "/dev/urandom", false);

  // create the correlator
  digital::correlate_access_code_tag_bb::sptr corr = digital::correlate_access_code_tag_bb::make(preamble, 0, "preamble");

  // create the deframer
  cc11xx::cc11xx_deframer_bb::sptr deframer = cc11xx::cc11xx_deframer_bb::make(sync1, sync2, false, true, 4);

  // create null sink
  blocks::null_sink::sptr sink = blocks::null_sink::make(sizeof(unsigned char));

  // connect the blocks together
  tb->connect(src, 0, conv, 0);
  tb->connect(conv, 0, corr, 0);

  //tb->connect(src, 0, corr, 0);
  tb->connect(corr, 0, deframer, 0);
  //tb->connect(deframer, 0, sink, 0);

  // Tell GNU Radio runtime to start flowgraph threads; the foreground thread
  // will block until either flowgraph exits (this example doesn't) or the
  // application receives SIGINT (e.g., user hits CTRL-C).
  //
  // Real applications may use tb->start() which returns, allowing the foreground
  // thread to proceed, then later use tb->stop(), followed by tb->wait(), to cleanup
  // GNU Radio before exiting.
  tb->run();

  // Exit normally.
  return 0;
}
