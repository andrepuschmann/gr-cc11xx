# Copyright 2011,2012 Free Software Foundation, Inc.
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

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

list(APPEND cc11xx_sources
    cc11xx_deframer_bb_impl.cc
    cc11xx_frame.cc
)

set(cc11xx_sources "${cc11xx_sources}" PARENT_SCOPE)
if(NOT cc11xx_sources)
    MESSAGE(STATUS "No C++ sources... skipping lib/")
    return()
endif(NOT cc11xx_sources)

add_library(gnuradio-cc11xx SHARED ${cc11xx_sources})
target_link_libraries(gnuradio-cc11xx ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES})
set_target_properties(gnuradio-cc11xx PROPERTIES DEFINE_SYMBOL "gnuradio_cc11xx_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-cc11xx PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-cc11xx
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_cc11xx_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/cc11xx_frame.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/test_cc11xx.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_cc11xx.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_cc11xx_deframer_bb.cc
)

add_executable(test-cc11xx ${test_cc11xx_sources})

target_link_libraries(
  test-cc11xx
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-cc11xx
)

GR_ADD_TEST(test_cc11xx test-cc11xx)
