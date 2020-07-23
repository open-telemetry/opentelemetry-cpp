#pragma once

#include "opentelemetry/context/context.h"
#include "opentelemetry/context/context_config.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

class RuntimeContext
{
public:
  // Return the current context.
  static Context *GetCurrent() { return context_handler_.GetCurrent(); }

  // Sets the current 'Context' object. Returns a token
  // that can be used to reset to the previous Context.
  static ContextToken Attach(Context *context) { return context_handler_.Attach(context); }

  // Resets the context to a previous value stored in the
  // passed in token. Returns true if successful, false otherwise
  static bool Detach(ContextToken &token) { return context_handler_.Detach(token); }

  static ContextType context_handler_;
};

ContextType RuntimeContext::context_handler_ = ContextType();

}  // namespace context
OPENTELEMETRY_END_NAMESPACE
