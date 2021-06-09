#pragma once

#ifdef __has_include
#  if __has_include("TraceLoggingDynamic.h")
#    include "TraceLoggingDynamic.h"
#    ifndef HAVE_TLD
#      define HAVE_TLD
#    endif
#  endif
#else
#  ifndef HAVE_TLD
#    define HAVE_TLD
#  endif
#  include "TraceLoggingDynamic.h"
#endif
