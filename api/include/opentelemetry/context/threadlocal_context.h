#ifndef THREAD_LOCAL_CONTEXT_H_
#define THREAD_LOCAL_CONTEXT_H_




#include "context.h"



/* An implementation of the runtime context that uses thread local storage*/
class ThreadLocalRuntimeContext :public RuntimeContext(){  

  


  class Token{
    Token(); 
  }

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
  attach(Context context)


  /* detach: Resets the context to a previous value.
   * 
   * Args:
   *  token: A reference to a previous context
   */
  void detach(Token token);

}


#endif  // THREAD_LOCAL_CONTEXT_H_
