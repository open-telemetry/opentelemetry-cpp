// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <string>
#include <utility>

#include "opentelemetry/context/context_value.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/version.h"

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
  Context(const T &keys_and_values) noexcept
      : head_{nostd::shared_ptr<DataList>{new DataList(keys_and_values)}}
  {}

  // Creates a context object from a key and value, this will
  // hold a shared_ptr to the head of the DataList linked list
  Context(nostd::string_view key, const ContextValue &value) noexcept
      : head_{nostd::shared_ptr<DataList>{new DataList(key, value)}}
  {}

  // Accepts a new iterable and then returns a new context that
  // contains the new key and value data. It attaches the
  // exisiting list to the end of the new list.
  template <class T>
  Context SetValues(T &values) const noexcept
  {
    Context context(values);
    auto last = context.head_;
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
  Context SetValue(nostd::string_view key, const ContextValue &value) const noexcept
  {
    Context context(key, value);
    context.head_->next_ = head_;
    return context;
  }

  // Returns the value associated with the passed in key.
  context::ContextValue GetValue(const nostd::string_view key) const noexcept
  {
    for (DataList *data = head_.get(); data != nullptr; data = data->next_.get())
    {
      if (key == data->key_)
      {
        return data->value_;
      }
    }
    return ContextValue{};
  }

  // Checks for key and returns true if found
  bool HasKey(const nostd::string_view key) const noexcept
  {
    return !nostd::holds_alternative<nostd::monostate>(GetValue(key));
  }

  bool operator==(const Context &other) const noexcept { return (head_ == other.head_); }

private:
  // A linked list to contain the keys and values of this context node
  struct DataList
  {
    std::string key_;

    nostd::shared_ptr<DataList> next_{nullptr};

    ContextValue value_;

    DataList() = default;

    // Builds a data list off of a key and value iterable and returns the head
    template <class T>
    DataList(const T &keys_and_vals)
    {
      auto iter = std::begin(keys_and_vals);
      if (iter == std::end(keys_and_vals))
        return;
      auto *node = this;
      *node      = DataList(iter->first, iter->second);
      for (++iter; iter != std::end(keys_and_vals); ++iter)
      {
        node->next_ = nostd::shared_ptr<DataList>(new DataList(iter->first, iter->second));
        node        = node->next_.get();
      }
    }

    // Builds a data list with just a key and value, so it will just be the head
    // and returns that head.
    DataList(nostd::string_view key, const ContextValue &value)
        : key_(key.begin(), key.end()), value_(value)
    {}
  };

  // Head of the list which holds the keys and values of this context
  nostd::shared_ptr<DataList> head_;
};
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
