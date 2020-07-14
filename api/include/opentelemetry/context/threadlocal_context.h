#pragma once

#include "opentelemetry/context/runtime_context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

  // The ThreadLocalContext class is a derived class from RuntimeContext and
  // provides a wrapper for propogating context through cpp thread locally.
  class ThreadLocalContext : public RuntimeContext
  {
    public:
      // The token class provides an identifier that is used by
      // the attach and detach methods to keep track of context
      // objects.
      class Token
      {
        private:
          friend class ThreadLocalContext;

          Context context_;

          // A constructor that sets the token's Context object to the
          // one that was passed in.
          Token(Context &context) { context_ = context; }

          // Returns the stored context object.
          Context GetContext() { return context_; }
      };

      // Return the current context.
      static Context GetCurrent() { return GetInstance(); }

      // Resets the context to a previous value stored in the
      // passed in token. Returns zero if successful, -1 otherwise
      static int Detach(Token &token)
      {
        // If the token context is the current context, return failure.
        GetInstance() = token.GetContext();
        return 0;
      }

      // Sets the current 'Context' object. Returns a token
      // that can be used to reset to the previous Context.
      static Token Attach(Context &context)
      {
        Token old_context = Token(GetInstance());
        GetInstance()     = context;
        return old_context;
      }

    private:
      // Provides storage and access to the thread_local context object.
      static Context &GetInstance()
      {
        static thread_local Context instance;
        return instance;
      }
  };
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
