#pragma once

#include <cstring>
#include "opentelemetry/context/context_value.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"

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
  Context() = default;
  // Creates a context object from a map of keys and identifiers, this will
  // hold a shared_ptr to the head of the DataList linked list
  template <class T>
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
  template <class T>
  Context SetValues(T &values) noexcept
  {
    Context context                  = Context(values);
    nostd::shared_ptr<DataList> last = context.head_;
    while (last->next_ != nullptr)
    {
      last = last->next_;
    }
    last->next_ = head_;
    return context;
  }

  // Accepts a new iterable and then returns a new context that
  // contains the new key and value data. It attaches the
  // exisiting list to the end of the new list.
  Context SetValue(nostd::string_view key, ContextValue value) noexcept
  {
    Context context      = Context(key, value);
    context.head_->next_ = head_;
    return context;
  }

  // Returns the value associated with the passed in key.
  context::ContextValue GetValue(const nostd::string_view key) noexcept
  {
    for (DataList *data = head_.get(); data != nullptr; data = data->next_.get())
    {
      if (key.size() == data->key_length_)
      {
        if (std::memcmp(key.data(), data->key_, data->key_length_) == 0)
        {
          return data->value_;
        }
      }
    }
    return (int64_t)0;
  }

  // Checks for key and returns true if found
  bool HasKey(const nostd::string_view key) noexcept
  {
    for (DataList *data = head_.get(); data != nullptr; data = data->next_.get())
    {
      if (key.size() == data->key_length_)
      {
        if (std::memcmp(key.data(), data->key_, data->key_length_) == 0)
        {
          return true;
        }
      }
    }
    return false;
  }

  bool operator==(const Context &other) { return (head_ == other.head_); }

private:
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
    template <class T>
    DataList(const T &keys_and_vals) : key_{nullptr}, next_(nostd::shared_ptr<DataList>{nullptr})
    {
      bool first = true;
      auto *node = this;
      for (auto &iter : keys_and_vals)
      {
        if (first)
        {
          *node = DataList(iter.first, iter.second);
          first = false;
        }
        else
        {
          node->next_ = nostd::shared_ptr<DataList>(new DataList(iter.first, iter.second));
          node        = node->next_.get();
        }
      }
    }

    // Builds a data list with just a key and value, so it will just be the head
    // and returns that head.
    DataList(nostd::string_view key, ContextValue value)
    {
      key_        = new char[key.size()];
      key_length_ = key.size();
      memcpy(key_, key.data(), key.size() * sizeof(char));
      value_ = value;
      next_  = nostd::shared_ptr<DataList>{nullptr};
    }

    DataList &operator=(DataList &&other)
    {
      key_length_ = other.key_length_;
      value_      = std::move(other.value_);
      next_       = std::move(other.next_);

      key_       = other.key_;
      other.key_ = nullptr;

      return *this;
    }

    ~DataList()
    {
      if (key_ != nullptr)
      {
        delete[] key_;
      }
    }
  };

  // Head of the list which holds the keys and values of this context
  nostd::shared_ptr<DataList> head_;
};
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
