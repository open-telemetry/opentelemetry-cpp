#pragma once

#include "context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

/* The RuntimeContext class provides a wrapper for
 * propogating context through cpp. */
class RuntimeContext
{
public:
  /* The token class provides an identifier that is used by
   * the attach and detach methods to keep track of context
   * objects.
   */
  class Token
  {
  private:
    friend class RuntimeContext;

    Context context_;

    /* A constructor that sets the token's Context object to the
     * one that was passed in.
     */
    Token(Context &context) { context_ = context; }

    /* Returns the stored context object. */
    Context GetContext() { return context_; }
  };

  /* Return the current context. */
  Context GetCurrent();

  /* Sets the current 'Context' object. Returns a token
   * that can be used to reset to the previous Context.
   */

  Token Attach(Context &context);

  /* Resets the context to a previous value stored in the
   * passed in token. Returns zero if successful, -1 otherwise
   */
  int Detach(Token &token);
};
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
