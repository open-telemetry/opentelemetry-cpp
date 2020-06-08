#pragma once

#include "context.h"

#include <string>

/* An implementation of the runtime context that uses thread local storage*/
class ThreadLocalRuntimeContext: public RuntimeContext(){  

  std::string _CONTEXT_KEY = "current_context";
  
  

  /* ThreadLocalRuntimeContext: Default constructor to set the 
   * current context to the thread local context 
   * 
   * Args: None
   */
  ThreadLocalRuntimeContext();

  /* attach: Sets the current 'Context' object. Returns a token 
   * that can be used to reset to the previous Context.
   * 
   * Args:
   *  context : the context to set. 
   */
  Token attach(Context context)

  /* get_current: Return the current context.
   *
   * Args: None 
   */
  Context get_current();  



  /* detach: Resets the context to a previous value.
   * 
   * Args:
   *  token: A reference to a previous context
   */
  void detach(Token token);

};

