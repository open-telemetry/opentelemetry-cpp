#pragma once

#include <atomic>
#include <iostream>

#include "opentelemetry/context/context_value.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/key_value_iterable_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

  // The context class provides a context identifier. Is built as a linked list
  // of context nodes which each have access to the nodes before it, but not
  // after. So each time a write is performed a new context is added to the list.
  // Within each context node there is another linked list which holds the key
  // and value data for each node.
  class Context
  {

    public:

      // Creates a context object from a map of keys and identifiers, this will
      // be the head of the context object linked list.
      template <class T, nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
        Context(const T &keys_and_values)
        {
          head_ = nostd::shared_ptr<DataList>{new DataList(keys_and_values)};
        }
      
        Context(nostd::string_view key, ContextValue value)
        {
          head_ = nostd::shared_ptr<DataList>{new DataList(key, value)};
        }

      // Accepts a new iterable and then returns a new context that
      // contains the new key and value data.
      template <class T, nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
        Context SetValues(T &values) noexcept
        {
          Context context = Context(values);
          context.head_->down_ = head_;
          return context;
        }
      
      // Accepts a new iterable and then returns a new context that
      // contains the new key and value data.
      Context SetValue(nostd::string_view key, ContextValue value) noexcept
      {
          Context context = Context(key, value);
          context.head_->down_ = head_;
          return context;
      }
      
      // Returns the value associated with the passed in key.
      context::ContextValue GetValue(nostd::string_view key)
      {

        DataList * head = &*head_;
        for(DataList* down = &*(head->down_); head != nullptr; down = &*(head->down_)){
          for (DataList* data = head; data != nullptr; data = data->next_)
          {
            if (key.size() == data->key_length_)
            {
              if (memcmp(key.data(), data->key_, data->key_length_) == 0)
              {
                return data->value_;
              }
            }
          }
          head = down;
          if(head == nullptr){
            break;
          } 
        }
        return "";
      }
   
    private:
      Context() = default;

    // A linked list to contain the keys and values of this context node
    class DataList
    {
      public:
        DataList *next_;

        nostd::shared_ptr<DataList> down_;

        char *key_;

        size_t key_length_;

        ContextValue value_;

        DataList() { next_ = nullptr; }

        // Builds a data list off of a key and value iterable and returns the head
        template <class T,
                 nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
                   DataList(const T &keys_and_vals)
                   {
                     auto iter = std::begin(keys_and_vals);
                     // Create list head
                     key_        = new char[nostd::string_view(iter->first).size()];
                     key_length_ = nostd::string_view(iter->first).size();
                     memcpy(key_, nostd::string_view(iter->first).data(),
                         nostd::string_view(iter->first).size() * sizeof(char));
                     value_ = iter->second;
                     next_  = nullptr;
                     down_ = nostd::shared_ptr<DataList>{nullptr};
                     ++iter;

                     DataList *previous_node = this;
                     // Iterate over the keys and values iterable and add nodes
                     for (; iter != std::end(keys_and_vals); ++iter)
                     {
                       DataList *node    = new DataList();
                       node->next_       = nullptr;
                       node->down_ = nostd::shared_ptr<DataList>{nullptr};
                       node->key_        = new char[nostd::string_view(iter->first).size()];
                       node->key_length_ = nostd::string_view(iter->first).size();
                       memcpy(node->key_, nostd::string_view(iter->first).data(),
                           nostd::string_view(iter->first).size() * sizeof(char));
                       node->value_         = iter->second;
                       previous_node->next_ = node;
                       previous_node        = node;
                     }
                   }

        // Builds a data list with just a key and value, so it will just be the head
        // and returns that head.
        DataList(nostd::string_view key, ContextValue value)
        {
          key_        = new char[nostd::string_view(key).size()];
          key_length_ = nostd::string_view(key).size();
          memcpy(key_, nostd::string_view(key).data(), nostd::string_view(key).size() * sizeof(char));
          value_ = value;
          next_  = nullptr;
          down_ = nostd::shared_ptr<DataList>{nullptr};
        }

        ~DataList() { 
          delete[] key_;
          delete next_;
         }
    };

    // Head of the list which holds the keys and values of this context
    nostd::shared_ptr<DataList> head_;

  };
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
