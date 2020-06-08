

class Context{
  public:
    std::map<std::string, int> _ctx;
    Context(std::map<std::string, int> ctx);  
    Context operator[] (std::string i);    
}

Context::Context(std::map<std::string,int> ctx){
  _ctx=ctx;
}

Context operator[](std::string){
  throw "Value Error";  
}



/* The RuntimeContext class provides a wrapper for 
 * propogating context through cpp*/
class RuntimeContext {
  public:
    
    /* attach: Sets the current 'Context' object. Returns a token 
     * that can be used to reset to the previous Context.
     * 
     * Args:
     *  context : the conext to set. 
     */
    virtual int attach(RuntimeContext context);
    

    /* get_current: Return the current context.
     *
     * Args: None 
     */
    virtual RuntimeContext get_current();  


    /* detach: Resets the context to a previous value.
     * 
     * Args:
     *  token: A reference to a previous context
     */
    virtual void detach(int);
  
  
}
