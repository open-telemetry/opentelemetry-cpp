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
    bool operator==(const Context &other) { return context_ == other; }

    ~Token() { Detach(*this); }

  private:
    friend class RuntimeContext;

    // A constructor that sets the token's Context object to the
    // one that was passed in.
    Token(Context context) { context_ = context; }

    Token() = default;

    Context context_;

    // Returns the stored context object.
    Context GetContext() { return context_; }
  };

  // Return the current context.
  static Context GetCurrent() { return context_handler_->InternalGetCurrent(); }

  // Sets the current 'Context' object. Returns a token
  // that can be used to reset to the previous Context.
  static Token Attach(Context context) { return context_handler_->InternalAttach(context); }

  // Resets the context to a previous value stored in the
  // passed in token. Returns true if successful, false otherwise
  static bool Detach(Token &token) { return context_handler_->InternalDetach(token); }

  static RuntimeContext *context_handler_;

  // Sets the Key and Value into the passed in context or if the context* is null,
  // the RuntimeContext.
  static Context SetValue(nostd::string_view key, ContextValue value, Context *context)
  {
    Context temp_context;
    if (context == nullptr)
    {
      temp_context = GetCurrent();
    }
    else
    {
      temp_context = *context;
    }
    Context written_context = temp_context.SetValue(key, value);
    return written_context;
  }

  // Returns the value associated with the passed in key and either the
  // passed in context* or the runtime context if the context* is null.
  static ContextValue GetValue(nostd::string_view key, Context *context)
  {
    Context temp_context;
    if (context == nullptr)
    {
      temp_context = GetCurrent();
    }
    else
    {
      temp_context = *context;
    }
    return temp_context.GetValue(key);
  }

protected:
  // Provides a token with the passed in context
  Token CreateToken(Context context) { return Token(context); }

  virtual Context InternalGetCurrent() = 0;

  virtual Token InternalAttach(Context context) = 0;

  virtual bool InternalDetach(Token &token) = 0;
};
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
