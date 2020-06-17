#pragma once

#include <bits/stdc++.h>
#include <map>
#include <vector>

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/version.h"
#include "opentelemetry/trace/key_value_iterable_view.h"
#include "opentelemetry/trace/key_value_iterable.h"
#include "opentelemetry/context/key_value_iterable_modifiable.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{


  /*The Key class is used to obscure access from the 
   * user to the context map. The identifier is used as a key 
   * to the context map.
   */ 
  class Key
  {

    public:

      /* GetIdentifier: returns the identifier */
      Key* GetIdentifier() { return identifier_; }


      /* Constructs a new Key with the passed in name */
      Key(nostd::string_view key_name)
      {
        key_name_ = key_name;
        identifier_ = this;
      }

    private:
      template<class T>
        friend class Context;

      nostd::string_view key_name_;

      Key* identifier_;


      /*
         Key(nostd::string_view key_name, int identifier)
         {
         key_name_   = key_name;
         identifier_ = this;
         }
       */
  };

  /* The context class provides a context identifier */
  template<class T>
    class Context
    {

      public:

        /* Creates a context object with no key/value pairs */
        Context() = default;

        /* Contructor, creates a context object from a map
         * of keys and identifiers
         */
        Context(const T &attributes) { 
          KeyValueIterableModifiable<T> iterable{attributes};
          key_val_map = iterable; 
        }

        /* Contructor, creates a context object from a 
         * KeyValueIterableModfiable object.
         */
        Context(const KeyValueIterableModifiable<T> iterable) { 
          key_val_map = iterable; 
        }

        /* Accepts a new iterable and then returns a new
         * context that contains both the original pairs 
         * and the new pair.
         */
        Context WriteValues(const T &attributes) noexcept
        {

          KeyValueIterableModifiable<T> iterable = attributes;       
          iterable.Add(key_val_map.GetData());
          return Context(iterable.GetData());
        }
        
        /* Returns the value associated with the passed in key */
        common::AttributeValue GetValue(Key key) { 
          return key_val_map.Get(key.GetIdentifier());
        }

      private:
        KeyValueIterableModifiable<T> key_val_map;

    };
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
