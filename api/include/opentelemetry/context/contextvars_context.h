#pragma once

#include "context.h"
#include <string>


/* An implementation of the RuntimeContext interface which wraps 
 * ContextVar under the hood */
class ContextVarsRuntimeContext: public RuntimeContext{

  std::string _CONTEXT_KEY = "current_context";


  /* ContextVarsRuntimeContext: Default constructor to set the 
   * current context to the thread local context 
   * 
   * Args: None
   */
  ContextVarsRuntimeContext();


  /* attach: Sets the current 'Context' object. Returns a token 
   * that can be used to reset to the previous Context.
   * 
   * Args:
   *  context : the context to set. 
   */
  Context attach(Context context);


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
  Token detach(Token token);

};



