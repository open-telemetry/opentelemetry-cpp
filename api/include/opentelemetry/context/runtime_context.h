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
    bool operator==(const Context &other) { return (*context_ == other); }

  private:
    friend class RuntimeContext;

    // A constructor that sets the token's Context object to the
    // one that was passed in.
    Token(Context *context) { context_ = context; }

    Token() = default;

    Context *context_;

    // Returns the stored context object.
    Context *GetContext() { return context_; }
  };

  // Return the current context.
  static Context *GetCurrent() { return context_handler_->InternalGetCurrent(); }

  // Sets the current 'Context' object. Returns a token
  // that can be used to reset to the previous Context.
  static Token Attach(Context *context) { return context_handler_->InternalAttach(context); }

  // Resets the context to a previous value stored in the
  // passed in token. Returns true if successful, false otherwise
  static bool Detach(Token &token) { return context_handler_->InternalDetach(token); }

  static RuntimeContext *context_handler_;

protected:
  // Provides a token with the passed in context
  Token CreateToken(Context *context) { return Token(context); }

  // Dummy method to be overriden by derived class implementation
  virtual Context *InternalGetCurrent() { return nullptr; }

  // Dummy method to be overriden by derived class implementation
  virtual Token InternalAttach(Context *context) { return Token(); }

  // Dummy method to be overriden by derived class implementation
  virtual bool InternalDetach(Token &token) { return false; }
};
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
