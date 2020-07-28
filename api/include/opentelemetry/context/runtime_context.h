#pragma once

#include "opentelemetry/context/context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{
// Provides a wrapper for propagating the context object globally. In order
// to use either the threadlocal_context.h file must be included or another
// implementation which must be derived from the RuntimeContext can be
// provided.
class RuntimeContext
{
public:
  class Token
  {
  public:
    bool operator==(const Context &other) noexcept { return context_ == other; }

    ~Token() noexcept { Detach(*this); }

  private:
    friend class RuntimeContext;

    // A constructor that sets the token's Context object to the
    // one that was passed in.
    Token(Context context) noexcept : context_(context){};

    Token() noexcept = default;

    Context context_;
  };

  // Return the current context.
  static Context GetCurrent() noexcept { return context_handler_->InternalGetCurrent(); }

  // Sets the current 'Context' object. Returns a token
  // that can be used to reset to the previous Context.
  static Token Attach(Context context) noexcept
  {
    return context_handler_->InternalAttach(context);
  }

  // Resets the context to a previous value stored in the
  // passed in token. Returns true if successful, false otherwise
  static bool Detach(Token &token) noexcept { return context_handler_->InternalDetach(token); }

  static RuntimeContext *context_handler_;

protected:
  // Provides a token with the passed in context
  Token CreateToken(Context context) noexcept { return Token(context); }

  virtual Context InternalGetCurrent() noexcept = 0;

  virtual Token InternalAttach(Context context) noexcept = 0;

  virtual bool InternalDetach(Token &token) noexcept = 0;
};
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
