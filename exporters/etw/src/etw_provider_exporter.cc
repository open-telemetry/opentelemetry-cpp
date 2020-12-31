#ifdef _WIN32

/* TODO: this definition needs to be removed when TraceLoggingDynamic.h is OSS */
#  ifndef HAVE_NO_TLD
#  define HAVE_NO_TLD
#  endif

#  include "opentelemetry/exporters/etw/etw_provider_exporter.h"

#endif
