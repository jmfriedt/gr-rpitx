/* -*- c++ -*- */

#define RPITX_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "rpitx_swig_doc.i"

%{
#include "rpitx/rpitx_sink.h"
%}

%include "rpitx/rpitx_sink.h"
GR_SWIG_BLOCK_MAGIC2(rpitx, rpitx_sink);
