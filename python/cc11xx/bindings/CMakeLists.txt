# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT cc11xx_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT cc11xx_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND cc11xx_python_files
    cc11xx_deframer_bb_python.cc
    python_bindings.cc)

GR_PYBIND_MAKE_OOT(cc11xx
   ../../..
   gr::cc11xx
   "${cc11xx_python_files}")

# copy in bindings .so file for use in QA test module
add_custom_target(
  copy_bindings_for_tests ALL
  COMMAND
    ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/*.so"
    ${CMAKE_BINARY_DIR}/test_modules/gnuradio/cc11xx/
  DEPENDS cc11xx_python)

install(TARGETS cc11xx_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/cc11xx COMPONENT pythonapi)
