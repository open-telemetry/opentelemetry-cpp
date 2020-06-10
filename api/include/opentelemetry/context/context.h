#pragma once


#include <map>
#include <string>
#include <mutex>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

  std::mutex context_id_mutex;

  /*The context class provides a context identifier*/
  class Context{

    public:

      /*The ContextKey class is used to obscure access from the 
       * user to the context map. The identifier is used as a key 
       * to the context map.
       */
      class ContextKey{
        private:
          friend class Context;  
          
          std::string key_name_;

          int identifier_;


          /* GetIdentifier: returns the identifier*/
          int GetIdentifier(){
            return identifier_;
          }

          /* Constructs a new ContextKey with the passed in name and 
           * identifier.
           */
          ContextKey(std::string key_name, int identifier){
            key_name_ = key_name;
            identifier_ = identifier;
          }

        public:
          
          /* Consructs a new ContextKey with the passed in name and increments
           * the identifier then assigns it to be the key's identifier.
           */
          ContextKey(std::string key_name){
            key_name_ = key_name;

            context_id_mutex.lock();
            
            Context::last_key_identifier_++;

            identifier_ = Context::last_key_identifier_;
            
            context_id_mutex.unlock();
          }

      };


      /* Creates a context object with no key/value pairs
       */
      Context(){
        ctx_map_ = std::map<int,int> {};

      }

      /* Contructor, creates a context object from a map
       * of keys and identifiers
       */
      Context(ContextKey key, int value){
        ctx_map_[key.GetIdentifier()] = value;
      } 


      /* Accepts a new key/value pair and then returns a new
       * context that contains both the original pairs and the new pair.
       */
      Context WriteValue(ContextKey key, int value){
        std::map<int, int> temp_map = ctx_map_;

        temp_map[key.GetIdentifier()] = value;

        return Context(temp_map);
      } 


      /* Returns the value associated with the passed in key */
      int GetValue(ContextKey key){
        return ctx_map_[key.GetIdentifier()];
      } 

      /* Returns a ContextKey that has the passed in name and the
       * next available identifier.*/
      ContextKey CreateKey(std::string key_name){
        int id;
        
        context_id_mutex.lock();
        
        last_key_identifier_++;
        
        id = last_key_identifier_;
        
        context_id_mutex.unlock();
            
        return ContextKey(key_name,id); 
      } 

    private:

      /*The identifier itself*/
      std::map<int, int> ctx_map_;

      /*Used to track that last ContextKey identifier and create the next one */
      static int last_key_identifier_;
      
      /* A constructor that accepts a key/value map*/
      Context(std::map<int,int> ctx_map){
        ctx_map_ = ctx_map;
      } 


  };



  /* The token class provides an identifier that is used by
   * the attach and detach methods to keep track of context 
   * objects.*/

  class Token{

    public:
      
      /* A constructor that sets the token's Context object to the
       * one that was passed in. 
       */
      Token(Context &ctx){
        ctx_ = ctx;  
      } 

      /* Returns the stored context object */
      Context GetContext(){
        return ctx_;
      }

    private:

      Context ctx_;
  };


  /* The RuntimeContext class provides a wrapper for 
   * propogating context through cpp*/
  class RuntimeContext {

    public:


      /* A default constructor that will set the context to
       * an empty context object.
       */
      RuntimeContext(){
        context_ = Context();
      }

      /* A constructor that will set the context as the passed in context.*/
      RuntimeContext(Context &context){ 

        context_ = context;
      }

      /* Sets the current 'Context' object. Returns a token 
       * that can be used to reset to the previous Context.
       */
      Token Attach(Context &context){

        Token old_context_token = Token(context_);

        context_ = context;

        return old_context_token;

      }


      /* Return the current context.
       */
      static Context GetCurrent(){
        Context context = context_;
        return context_;  
      }


      /* Resets the context to a previous value stored in the 
       * passed in token. 
       */
      int Detach(Token &token){
        context_ = token.GetContext();      
      }


    private:
     
      static thread_local Context context_;

  };
  
  thread_local Context RuntimeContext::context_ = Context();
  int Context::last_key_identifier_ = 0;


}
OPENTELEMETRY_END_NAMESPACE
