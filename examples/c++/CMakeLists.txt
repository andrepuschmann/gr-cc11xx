# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

include_directories(
    ${GR_AUDIO_INCLUDE_DIRS}
    ${GR_ANALOG_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)



add_executable(deframer deframer.cpp)
target_link_libraries(deframer
  gnuradio-runtime gnuradio-audio gnuradio-analog gnuradio-blocks gnuradio-digital gnuradio-cc11xx boost_system)

add_executable(file_source_test file_source_test.cpp)
target_link_libraries(file_source_test
  gnuradio-runtime gnuradio-audio gnuradio-analog gnuradio-filter gnuradio-blocks gnuradio-digital gnuradio-cc11xx boost_system)

#INSTALL(TARGETS
#    deframer
#    DESTINATION ${GR_gnuradio-homematic}
#    COMPONENT "homematic_examples"
#)
