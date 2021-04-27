#pragma once

#ifdef __has_include
#  if __has_include("TraceLoggingDynamic.h")
#    include "TraceLoggingDynamic.h"
#    ifndef HAVE_TLD
#      define HAVE_TLD
#    endif
#  endif
#else
#  ifdef HAVE_TLD
#    include "TraceLoggingDynamic.h"
#  endif
#endif
