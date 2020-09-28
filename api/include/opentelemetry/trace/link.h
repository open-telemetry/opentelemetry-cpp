#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/trace/key_value_iterable.h"
#include "opentelemetry/trace/span_context.h"

OPENTELEMETRY_BEGIN_NAMESPACE 
namespace trace
{
class Link 
{
public:

    //returns the Span Context associated with this Link
    virtual const SpanContext& GetContext() const = 0 ;

    // returns the attributes associated with link
    virtual const KeyValueIterable& GetAttributes() const = 0;

};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE