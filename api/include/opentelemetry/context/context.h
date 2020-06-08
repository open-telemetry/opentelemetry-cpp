#pragma once

/*The context class provides a context identifier*/
class Context{
  public:

    /*The identifier itself*/
    std::map<std::string, int> _ctx;

    /* Context: contructor 
     * 
     * Args: 
     *  ctx: a map containing the context identifier
     */
    Context(std::map<std::string, int> ctx);  

    /* Context operator[]: Prevents the modification of the identifier
     *
     * Args: none 
     */
    Context operator[] (std::string str);    
};

/* The token class provides:????*/
class Token{
  Token(); 
};



/* The RuntimeContext class provides a wrapper for 
 * propogating context through cpp*/
class RuntimeContext {
  public:

    /* attach: Sets the current 'Context' object. Returns a token 
     * that can be used to reset to the previous Context.
     * 
     * Args:
     *  context : the context to set. 
     */
    static Token attach(Context context);


    /* get_current: Return the current context.
     *
     * Args: None 
     */
    static Context get_current();  


    /* detach: Resets the context to a previous value.
     * 
     * Args:
     *  token: A reference to a previous context
     */
    static void detach(Token token);


};



