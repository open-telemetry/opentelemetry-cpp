#pragma once

#include "context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

  // The RuntimeContext class provides a wrapper for
  // propogating context through cpp.
  class RuntimeContext
  {
    public:
      // The token class provides an identifier that is used by
      // the attach and detach methods to keep track of context
      // objects.
      class Token;

      // Return the current context.
      Context GetCurrent();

      // Sets the current 'Context' object. Returns a token
      // that can be used to reset to the previous Context.
      Token Attach(Context &context);

      // Resets the context to a previous value stored in the
      // passed in token. Returns zero if successful, -1 otherwise
      int Detach(Token &token);
  };
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
