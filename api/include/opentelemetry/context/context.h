#pragma once

#include <map>
#include <string>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{


/*The context class provides a context identifier*/
class Context{
  private:
    /*The identifier itself*/
    std::map<std::string, int> ctx_;

  public:
    /* Context: contructor, creates a context object from a map
     * of keys and identifiers
     */
    Context(std::map<std::string, int> ctx);  

};

/* The token class provides an identifier that is used by
 * the RuntimeContext attach and detach methods to keep track of context 
 * objects.*/
class Token{
  Token(); 
};


/* The RuntimeContext class provides a wrapper for 
 * propogating context through cpp*/
class RuntimeContext {
  public:

    /* RuntimeContext: A constructor that will set the current
     * context to the threading local.
     */
    RuntimeContext();

    /* attach: Sets the current 'Context' object. Returns a token 
     * that can be used to reset to the previous Context.
     */
    static Token Attach(Context context);


    /* get_current: Return the current context.
     */
    static Context GetCurrent();  


    /* detach: Resets the context to a previous value.
     */
    static void Detach(Token token);


};

}
