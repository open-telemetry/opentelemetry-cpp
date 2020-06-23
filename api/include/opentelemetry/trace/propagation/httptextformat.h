#pragma once

#include <cstdint>
#include "opentelemetry/context/context.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{

virtual static Context SetSpanInContext(Span span, Context context = NULL)
virtual static Span GetCurrentSpan(Context context = NULL)

template <typename T>
class HTTPTextFormat {
    /** This class provides an interface that enables extracting and injecting
    context into headers of HTTP requests. HTTP frameworks and clients
    can integrate with HTTPTextFormat by providing the object containing the
     headers, and a getter and setter function for the extraction and
    injection of values, respectively.*/
    public:
        using Setter = nostd::string_view(*)(nostd::string_view,nostd::string_view);
        using Getter = void(*)(T&,nostd::string_view,nostd::string);
        virtual Context extract(Setter get_from_carrier, const T &carrier, Context &context)
        virtual void inject(Getter set_from_carrier, T &carrier, const Context &context)
};
}
}
OPENTELEMETRY_END_NAMESPACE