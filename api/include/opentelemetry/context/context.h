#pragma once

#include <atomic>

#include "opentelemetry/context/context_value.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/key_value_iterable_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

  // The context class provides a context identifier. Is built as a linked list
  // of DataList nodes and each context holds a shared_ptr to a place within 
  // the list that determines which keys and values it has access to. All that
  // come before and none that come after.
  class Context
  {

    public:

      // Creates a context object from a map of keys and identifiers, this will
      // hold a shared_ptr to the head of the DataList linked list 
      template <class T, nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
        Context(const T &keys_and_values)
        {
          head_ = nostd::shared_ptr<DataList>{new DataList(keys_and_values)};
        }
      
      // Creates a context object from a key and value, this will
      // hold a shared_ptr to the head of the DataList linked list 
        Context(nostd::string_view key, ContextValue value)
        {
          head_ = nostd::shared_ptr<DataList>{new DataList(key, value)};
        }

      // Accepts a new iterable and then returns a new context that
      // contains the new key and value data. It attaches the
      // exisiting list to the end of the new list.
      template <class T, nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
        Context SetValues(T &values) noexcept
        {
          Context context = Context(values);
          nostd::shared_ptr<DataList> last_node; 
          for(nostd::shared_ptr<DataList> data = context.head_; data != nullptr; data = data->next_){
            last_node = data;
          }
          last_node->next_ = head_;
          return context;
        }
      
      // Accepts a new iterable and then returns a new context that
      // contains the new key and value data. It attaches the
      // exisiting list to the end of the new list.
      Context SetValue(nostd::string_view key, ContextValue value) noexcept
      {
          Context context = Context(key, value);
          context.head_->next_ = head_;
          return context;
      }
      
      // Returns the value associated with the passed in key.
      context::ContextValue GetValue(nostd::string_view key)
      {
          for (nostd::shared_ptr<DataList> data = head_; data != nullptr; data = data->next_)
          {
            if (key.size() == data->key_length_)
            {
              if (memcmp(key.data(), data->key_, data->key_length_) == 0)
              {
                return data->value_;
              }
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
        nostd::shared_ptr<DataList> next_;

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
                     next_  = nostd::shared_ptr<DataList>{nullptr};
                     ++iter;

                     DataList* previous_node = this;
                     // Iterate over the keys and values iterable and add nodes
                     for (; iter != std::end(keys_and_vals); ++iter)
                     {
                       nostd::shared_ptr<DataList> node    = nostd::shared_ptr<DataList>{new DataList()};
                       node->next_       = nostd::shared_ptr<DataList>{nullptr};
                       node->key_        = new char[nostd::string_view(iter->first).size()];
                       node->key_length_ = nostd::string_view(iter->first).size();
                       memcpy(node->key_, nostd::string_view(iter->first).data(),
                           nostd::string_view(iter->first).size() * sizeof(char));
                       node->value_         = iter->second;
                       previous_node->next_ = node;
                       previous_node        = &*node;
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
          next_  = nostd::shared_ptr<DataList>{nullptr};
        }

        ~DataList() { 
          delete[] key_;
         }
    };

    // Head of the list which holds the keys and values of this context
    nostd::shared_ptr<DataList> head_;

  };
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
