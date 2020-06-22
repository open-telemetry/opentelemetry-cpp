#pragma once

#include <map>
#include <vector>
#include <sstream> 
#include <iostream>
#include <atomic>

#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/version.h"
#include "opentelemetry/trace/key_value_iterable_view.h"
#include "opentelemetry/trace/key_value_iterable.h"
#include "opentelemetry/trace/tracer.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

  /* The context class provides a context identifier */
  template<class T>
    class Context
    {

      public:

        /*The Key class is used to obscure access from the 
         * user to the context map. The identifier is used as a key 
         * to the context map.
         */ 
        class Key
        {

          public:
            /*Returns the key's identifier*/            
            nostd::string_view GetIdentifier() {return nostd::string_view(identifier_);}
            /*Returns the key's name */
            nostd::string_view GetName() { return key_name_;}

          private:
            friend class Context;

            /* Constructs a new Key with the passed in name. Sets the identifier as
             * the address of this object. */
            Key(nostd::string_view key_name) : key_name_{key_name}
            {
              std::stringstream ss;
              ss << (void *)this;
              nostd::string_view temp_view;
              temp_view = ss.str();

              memcpy(identifier_, temp_view.data(), temp_view.size());
            }

            nostd::string_view key_name_;

            char identifier_ [50];
        };

        /* Creates a key with the passed in name and returns it. */
        Key CreateKey(nostd::string_view key_name){
          return Key(key_name); 
        }

        /* Contructor, creates a context object from a map of keys 
         * and identifiers.
         */
        Context(const T &attributes):  key_vals_(attributes) { 
          /* Currently only used as a check, to ensure T is of the right type. */
          trace::KeyValueIterableView<T> iterable = trace::KeyValueIterableView<T>(attributes);
        }

        Context() { 
        }

        /* Accepts a new iterable and then returns a new  context that
         * contains both the original pairs and the new pair.
         */
        Context WriteValues(T &attributes) noexcept
        {
          /* Currently only used as a check, to ensure T is of the right type. */
          trace::KeyValueIterableView<T> iterable = trace::KeyValueIterableView<T>(attributes);

          std::insert_iterator<T> back (attributes, std::begin(attributes));

          for (auto iter = std::begin(key_vals_); iter != std::end(key_vals_); ++iter)
          {
            back = *iter;
          }

          return Context(attributes);
        }

        /* Returns the value associated with the passed in key */
        common::AttributeValue GetValue(Key key) { 

          for (auto iter = std::begin(key_vals_); iter != std::end(key_vals_); ++iter)
          {
            if(key.GetIdentifier() == iter->first){
              return iter->second;
            }
          }

          return "";
        }
       
        /* Iterates through the current and comparing context objects 
         * to check for equality, */
        bool operator== (const Context &other){
          
          /*Check for case where either both or one object has no contents. */
          if(std::begin(other.key_vals_) == std::end(other.key_vals_)){
            if(std::begin(key_vals_) == std::end(key_vals_)){
              return true;
            }
            else{
              return false;
            }
          }

          if(std::begin(key_vals_) == std::end(key_vals_)){
            return false;
          }

             
          /*Compare the contexts*/       
          for (auto iter = std::begin(key_vals_); iter != std::end(key_vals_); ++iter)
          {
            int found = 0;
            
            for(auto iter_other = std::begin(other.key_vals_); iter_other != std::end(other.key_vals_); iter_other++){
                if(iter->first == iter_other->first){
                 if(nostd::get<nostd::string_view>(iter->second) == nostd::get<nostd::string_view>(iter_other->second)){
                    found = 1;
                    break;
                  }
                } 
            }
            
            if(found == 0){
              return false;
            }
          }
          
          return true;
        }

        /* Copy Constructors. */
        Context(const Context& other) = default;
        Context& operator=(const Context& other) = default;

      private:
        T key_vals_;

    };

}  // namespace context
OPENTELEMETRY_END_NAMESPACE
