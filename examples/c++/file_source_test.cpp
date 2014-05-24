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

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <gnuradio/top_block.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/blocks/file_source.h>
#include <gnuradio/blocks/throttle.h>
#include <gnuradio/analog/simple_squelch_cc.h>
#include <gnuradio/analog/quadrature_demod_cf.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/filter/fir_filter_ccf.h>
#include <gnuradio/digital/clock_recovery_mm_ff.h>
#include <gnuradio/digital/binary_slicer_fb.h>
#include <gnuradio/digital/correlate_access_code_tag_bb.h>
#include "cc11xx/cc11xx_deframer_bb.h"

using namespace gr;

int main(int argc, char **argv)
{
    std::string preamble = "10101010101010101010101010101010";
    unsigned char sync1 = 0xe9;
    unsigned char sync2 = 0xca;

    double fs = 1e6;
    double filter_gain = 1;

    double filter_decim = 20;
    double filter_cutoff = 100e3;

    float omega = 5.0;
    float gain_omega = 0.0076;
    float mu = 0.5;
    float gain_mu = 0.175;

    double squelch_threshold = -10;
    double squelch_alpha = 0.5;

    std::vector<float> taps = filter::firdes::low_pass(filter_gain, fs, filter_cutoff, filter_cutoff / 2);

    // Construct a top block that will contain flowgraph blocks.  Alternatively,
    // one may create a derived class from top_block and hold instantiated blocks
    // as member data for later manipulation.
    top_block_sptr tb = make_top_block("cc11xx_deframer");

    // get input file
    struct passwd *pw = getpwuid(getuid());
    std::string filename(pw->pw_dir);
    filename += "/gnuradio/raw_cc1100_1msps.dat";

    // create the blocks
    blocks::file_source::sptr src = blocks::file_source::make(8, filename.c_str(), false);
    blocks::throttle::sptr throttle = blocks::throttle::make(8, fs);
    filter::fir_filter_ccf::sptr lp = filter::fir_filter_ccf::make(filter_decim, taps);
    analog::simple_squelch_cc::sptr squelch = analog::simple_squelch_cc::make(squelch_threshold, squelch_alpha);
    analog::quadrature_demod_cf::sptr demod = analog::quadrature_demod_cf::make(0.2);

    digital::clock_recovery_mm_ff::sptr clkrec = digital::clock_recovery_mm_ff::make(omega, gain_omega, mu, gain_mu, 0.005);
    digital::binary_slicer_fb::sptr slicer = digital::binary_slicer_fb::make();
    digital::correlate_access_code_tag_bb::sptr corr = digital::correlate_access_code_tag_bb::make(preamble, 0, "preamble");
    cc11xx::cc11xx_deframer_bb::sptr deframer = cc11xx::cc11xx_deframer_bb::make(sync1, sync2, true, true, 4);

    // connect the blocks together
    tb->connect(src, 0, throttle, 0);
    tb->connect(throttle, 0, lp, 0);
    tb->connect(lp, 0, squelch, 0);
    tb->connect(squelch, 0, demod, 0);
    tb->connect(demod, 0, clkrec, 0);
    tb->connect(clkrec, 0, slicer, 0);
    tb->connect(slicer, 0, corr, 0);
    tb->connect(corr, 0, deframer, 0);

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
