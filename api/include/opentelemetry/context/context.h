#pragma once

#include <bits/stdc++.h>
#include <atomic>
#include <map>
#include <vector>

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/version.h"
#include "opentelemetry/trace/key_value_iterable_view.h"
#include "opentelemetry/trace/key_value_iterable.h"
#include "opentelemetry/context/key_value_iterable_modifiable.h"
//#include "opentelemetry/context/threadlocal_context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

  /*The context class provides a context identifier */

  class Key
  {
    private:
        template<class T>
        friend class Context;

      nostd::string_view key_name_;

      Key* identifier_;



      Key(nostd::string_view key_name, int identifier)
      {
        key_name_   = key_name;
        identifier_ = this;
      }

    public:

      Key* GetIdentifier() { return identifier_; }


      Key(nostd::string_view key_name)
      {
        key_name_ = key_name;
        identifier_ = this;
      }
  };

    template<class T>
    class Context
    {

      public:


        Context() = default;
        
        Context(const T &attributes) { 
          KeyValueIterableModifiable<T> iterable{attributes};
          key_val_map = iterable; 
        }
        
        Context(const KeyValueIterableModifiable<T> iterable) { 
          key_val_map = iterable; 
        }
        
        Context WriteValues(const T &attributes) noexcept
        {

          KeyValueIterableModifiable<T> iterable = attributes;       
          iterable.Add(key_val_map.GetData());
          return Context(iterable.GetData());
        }

        common::AttributeValue GetValue(Key key) { 
          return key_val_map.Get(key.GetIdentifier());
        }

      private:
        KeyValueIterableModifiable<T> key_val_map;

    };



  template <class M>
  class Token;

  template<class M>
    class RuntimeContext
    {

      public:

        Context<M> GetCurrent();
        Token<M> Attach(Context<M> context);
        int Detach(Token<M> token);
    };

   
  template <class M>
   class ThreadLocalContext : public RuntimeContext<M>
   {
      public:
      
      static Context<M> GetCurrent(){
        return GetInstance();
      }

      int Detach(Token<M> token){
        GetInstance() = token.GetCtx();
        return 0;
      }

      Token<M> Attach(Context<M> context){
        Token<M> old_context = Token<M>(GetInstance());
        GetInstance() = context;
        return old_context;
      }

      private:
        
        static Context<M> &GetInstance(){
          static Context<M> instance;
          return instance;
        }
       
   };


  template <class M>
  class Token
  {

    private:
      friend class RuntimeContext<M>;
      friend class ThreadLocalContext<M>;

      Context<M> ctx_;

      Token(Context<M> ctx) { ctx_ = ctx; }


      Context<M> GetCtx() { return ctx_; }
  };

}  // namespace context
OPENTELEMETRY_END_NAMESPACE
