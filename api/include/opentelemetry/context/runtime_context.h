#pragma once

#include "context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

  /* The RuntimeContext class provides a wrapper for 
     * propogating context through cpp. */
  template<class M>
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
            friend class RuntimeContext<M>;

            Context<M> ctx_;

            /* A constructor that sets the token's Context object to the
             * one that was passed in. 
             */
            Token(Context<M> ctx) { ctx_ = ctx; }

            /* Returns the stored context object */
            Context<M> GetCtx() { return ctx_; }
        };

        /* Return the current context. */
        Context<M> GetCurrent();

        /* Sets the current 'Context' object. Returns a token 
         * that can be used to reset to the previous Context.
         */

        Token Attach(Context<M> context);

        /* Resets the context to a previous value stored in the 
         * passed in token. Returns zero if successful, -1 otherwise
         */
        int Detach(Token token);
    };


}
OPENTELEMETRY_END_NAMESPACE
