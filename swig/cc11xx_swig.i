/* -*- c++ -*- */

#define CC11XX_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "cc11xx_swig_doc.i"

%{
#include "cc11xx/cc11xx_deframer_bb.h"
%}


%include "cc11xx/cc11xx_deframer_bb.h"
GR_SWIG_BLOCK_MAGIC2(cc11xx, cc11xx_deframer_bb);
