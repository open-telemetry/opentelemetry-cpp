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
// of context nodes which each have access to the nodes before it, but not
// after. So each time a write is performed a new context is added to the list.
// Within each context node there is another linked list which holds the key
// and value data for each node.
class Context
{

public:
  nostd::shared_ptr<Context> *context_ptr_;

  // Creates a new context by calling a constructor with the passed in
  // keyValueIterable and returns a shared_ptr pointing to it.
  template <class T, nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
  static nostd::shared_ptr<Context> CreateContext(T &keys_and_values)
  {
    nostd::shared_ptr<Context> temp = nostd::shared_ptr<Context>{new Context(keys_and_values)};
    temp->context_ptr_              = &temp;
    return temp;
  }

  // Creates a new context by calling a constructor with the passed in
  // key and value and returns a shared_ptr pointing to it.
  static nostd::shared_ptr<Context> CreateContext(nostd::string_view key, ContextValue value)
  {
    nostd::shared_ptr<Context> temp = nostd::shared_ptr<Context>{new Context(key, value)};
    temp->context_ptr_              = &temp;
    return temp;
  }

  // Accepts a new iterable and then returns a new context that
  // contains the new key and value data.
  template <class T, nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
  nostd::shared_ptr<Context> SetValues(T &values) noexcept
  {
    nostd::shared_ptr<Context> temp = nostd::shared_ptr<Context>{new Context(values)};
    temp->context_ptr_              = &temp;
    temp->SetNext(*context_ptr_);
    return temp;
  }

  // Accepts a new iterable and then returns a new context that
  // contains the new key and value data.
  nostd::shared_ptr<Context> SetValue(nostd::string_view key, ContextValue value) noexcept
  {
    nostd::shared_ptr<Context> context = nostd::shared_ptr<Context>{new Context(key, value)};
    context->context_ptr_              = &context;
    context->SetNext(*context_ptr_);
    return context;
  }

  // Returns the value associated with the passed in key.
  context::ContextValue GetValue(nostd::string_view key)
  {
    // Iterate through the context nodes
    for (auto context = *context_ptr_; context != nullptr; context = context->next_)
    {
      // Iterate through the internal data nodes
      for (DataList *data = context->head_; data != nullptr; data = data->next_)
      {
        if (key.size() == data->key_length_)
        {
          if (memcmp(key.data(), data->key_, data->key_length_) == 0)
          {
            return data->value_;
          }
        }
      }
    }
    return "";
  }

  // Iterates through the current and comparing context objects
  // to check for equality.
  bool operator==(const Context &other)
  {
    // If the two contexts are not at the place in the list
    // they cannot be equivalent
    if (next_ != other.next_)
    {
      return false;
    }

    DataList *other_data = other.head_;

    // Iterate through the lists and compare the keys
    // TODO should also compare the values but the nostd::variant
    // seems to not have a comparison operator
    for (DataList *data = head_; data != nullptr; data = data->next_)
    {
      if (other_data == nullptr)
      {
        return false;
      }
      if (data->key_ != other_data->key_)
      {
        return false;
      }
      data       = data->next_;
      other_data = other_data->next_;
    }
    return true;
  }

  // Copy constructor links to the same next node but copies the other's
  // data list into a new linked list
  Context(const Context &other)
  {
    next_          = other.next_;
    head_          = new DataList;
    DataList *data = head_;
    // Copy the other data into a new linked list
    for (DataList *other_data = other.head_; other_data != nullptr; other_data = other_data->next_)
    {
      if (other_data->next_ != nullptr)
      {
        data->next_ = new DataList;
      }
      else
      {
        data->next_ = nullptr;
      }
      data->key_length_ = other_data->key_length_;
      data->key_        = new char[data->key_length_];
      memcpy(data->key_, other_data->key_, data->key_length_);
      data->value_ = other_data->value_;
      data         = data->next_;
    }
  }

  // Assignment constructor links to the same next node but copies the other's
  // data list into a new linked list
  void operator=(const Context &other)
  {
    next_          = other.next_;
    head_          = new DataList;
    DataList *data = head_;
    // Copy the other data into a new linked list
    for (DataList *other_data = other.head_; other_data != nullptr; other_data = other_data->next_)
    {
      if (other_data->next_ != nullptr)
      {
        data->next_ = new DataList;
      }
      else
      {
        data->next_ = nullptr;
      }
      data->key_length_ = other_data->key_length_;
      data->key_        = new char[data->key_length_];
      memcpy(data->key_, other_data->key_, data->key_length_);
      data->value_ = other_data->value_;
      data         = data->next_;
    }
  }

  ~Context()
  {
    for (DataList *data = head_; data != nullptr;)
    {
      DataList *next = data->next_;
      delete data;
      data = next;
    }
  }

  // Used to set the next node in the list
  void SetNext(nostd::shared_ptr<Context> next) { next_ = next; }

private:
  Context() = default;

  // A linked list to contain the keys and values of this context node
  class DataList
  {
  public:
    DataList *next_;

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
      ++iter;

      DataList *previous_node = this;
      // Iterate over the keys and values iterable and add nodes
      for (; iter != std::end(keys_and_vals); ++iter)
      {
        DataList *node    = new DataList();
        node->next_       = nullptr;
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
    }

    ~DataList() { delete[] key_; }
  };

  // Creates a context object from a map of keys and identifiers, this will
  // be the head of the context object linked list.
  template <class T, nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
  Context(const T &keys_and_values)
  {
    head_ = new DataList(keys_and_values);
    next_ = nostd::shared_ptr<Context>{nullptr};
  }

  // Creates a context object from a key and value, this will be the head
  // of the context object linked list.
  Context(nostd::string_view key, ContextValue value) { head_ = new DataList(key, value); }

  // Head of the list which holds the keys and values of this context
  DataList *head_ = nullptr;

  // Pointer to the next context object in the context list
  nostd::shared_ptr<Context> next_;
};
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
