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


#ifndef _QA_CC11XX_DEFRAMER_BB_H_
#define _QA_CC11XX_DEFRAMER_BB_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

namespace gr {
  namespace cc11xx {

    class qa_cc11xx_deframer_bb : public CppUnit::TestCase
    {
    public:
      CPPUNIT_TEST_SUITE(qa_cc11xx_deframer_bb);
      CPPUNIT_TEST(t1_crc);
      CPPUNIT_TEST(t2_whitening);
      CPPUNIT_TEST_SUITE_END();

    private:
      void t1_crc();
      void t2_whitening();
    };

  } /* namespace cc11xx */
} /* namespace gr */

#endif /* _QA_CC11XX_DEFRAMER_BB_H_ */

