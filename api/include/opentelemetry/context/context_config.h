#pragma once

//#include "opentelemetry/context/threadlocal_context.h"
#include "threadlocal_context.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{
// The ContextType is used to determine the implementation that will be used
// for the RuntimeContext. It defaults to the ThreadLocalContext object.
typedef ThreadLocalContext ContextType;

// The token type provides an identifier that is used by
// the attach and detach methods to keep track of context
// objects.
typedef ThreadLocalContext::Token ContextToken;
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
