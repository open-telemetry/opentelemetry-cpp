#ifndef CONTEXT_H_
#define CONTEXT_H_


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
    Context operator[] (std::string i);    
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
    virtual int attach(Context context);


    /* get_current: Return the current context.
     *
     * Args: None 
     */
    virtual Context get_current();  


    /* detach: Resets the context to a previous value.
     * 
     * Args:
     *  token: A reference to a previous context
     */
    virtual void detach(int);


};


#endif //CONTEXT_H_
