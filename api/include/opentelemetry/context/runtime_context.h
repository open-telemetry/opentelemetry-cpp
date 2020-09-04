#pragma once

#include "opentelemetry/context/iruntime_context.h"
#include "opentelemetry/context/threadlocal_context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

// Provides a wrapper for propagating the context object globally.
class RuntimeContext : public IRuntimeContext
{
public:
  // Return the current context.
  static Context GetCurrent() noexcept { return ContextHandler()->InternalGetCurrent(); }

  // Sets the current 'Context' object. Returns a token
  // that can be used to reset to the previous Context.
  static Token Attach(Context context) noexcept
  {
    return ContextHandler()->InternalAttach(context);
  }

  // Resets the context to a previous value stored in the
  // passed in token. Returns true if successful, false otherwise
  static bool Detach(Token &token) noexcept { return ContextHandler()->InternalDetach(token); }

  static inline IRuntimeContext *ContextHandler(IRuntimeContext *context_handler = nullptr)
  {
    static IRuntimeContext *context_handler_ =
        (context_handler != nullptr) ? context_handler : new ThreadLocalContext();
    return context_handler_;
  };

  // Sets the Key and Value into the passed in context or if a context is not
  // passed in, the RuntimeContext.
  // Should be used to SetValues to the current RuntimeContext, is essentially
  // equivalent to RuntimeContext::GetCurrent().SetValue(key,value). Keep in
  // mind that the current RuntimeContext will not be changed, and the new
  // context will be returned.
  static Context SetValue(nostd::string_view key,
                          ContextValue value,
                          Context *context = nullptr) noexcept
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
    return temp_context.SetValue(key, value);
  }

  // Returns the value associated with the passed in key and either the
  // passed in context* or the runtime context if a context is not passed in.
  // Should be used to get values from the current RuntimeContext, is
  // essentially equivalent to RuntimeContext::GetCurrent().GetValue(key).
  static ContextValue GetValue(nostd::string_view key, Context *context = nullptr) noexcept
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
  Token CreateToken(Context context) noexcept { return Token(context); }

  virtual Context InternalGetCurrent() noexcept = 0;

  virtual Token InternalAttach(Context context) noexcept = 0;

  virtual bool InternalDetach(Token &token) noexcept = 0;
};

inline Token::ContextDetacher::~ContextDetacher()
{
  context::Token token;
  token.context_ = context_;
  context::RuntimeContext::Detach(token);
}
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
