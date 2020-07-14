#pragma once

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/context/context_value.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/key_value_iterable_view.h"

#include <iostream>
#include <map>
#include <atomic>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

// The context class provides a context identifier.
// This is a dummy class that is meant to be overridden,
// the methods return default values.
class Context
{

public:
  Context(){
    context_id_ = GetNextAvailableId(); 
  }

  // Contructor, creates a context object from a map of keys
  // and identifiers.
  template <class T, nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
  Context(const T &keys_and_values)
  {
    trace::KeyValueIterableView<T> iterable{keys_and_values};
    iterable.ForEachKeyValue([&](nostd::string_view key, context::ContextValue value) noexcept {
      context_map_[std::string(key)] = value;
      return true;
    });
    context_id_ = GetNextAvailableId(); 
  }

  // Accepts a key and a value and then returns a new context that
  // contains both the original pairs and the new pair.
  template <class T>
  Context SetValue(nostd::string_view key, T &value) noexcept
  {
    std::map<std::string, context::ContextValue> context_map_copy;
    trace::KeyValueIterableView<std::map<std::string, context::ContextValue>> context_map_iterable{
        context_map_};

    context_map_iterable.ForEachKeyValue([&](nostd::string_view key,
                                             context::ContextValue value) noexcept {
      context_map_copy[std::string(key)] = value;
      return true;
    });

    context_map_copy[std::string(key)] = value;

    return Context(context_map_copy);
  }

  // Accepts a new iterable and then returns a new context that
  // contains both the original pairs and the new pair.
  template <class T, nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
  Context SetValues(T &keys_and_values) noexcept
  {
    std::map<std::string, context::ContextValue> context_map_copy;
    trace::KeyValueIterableView<std::map<std::string, context::ContextValue>> context_map_iterable{
        context_map_};

    context_map_iterable.ForEachKeyValue([&](nostd::string_view key,
                                             context::ContextValue value) noexcept {
      context_map_copy[std::string(key)] = value;
      return true;
    });

    trace::KeyValueIterableView<T> iterable{keys_and_values};

    iterable.ForEachKeyValue([&](nostd::string_view key, context::ContextValue value) noexcept {
      context_map_copy[std::string(key)] = value;
      return true;
    });

    return Context(context_map_copy);
  }

  // Returns the value associated with the passed in key.
  context::ContextValue GetValue(nostd::string_view key) { return context_map_[std::string(key)]; }

  // Copy Constructors.
  Context(const Context &other) = default;
  Context &operator=(const Context &other) = default;
  
  // Comparison operator.
  bool operator==(const Context &other){
    return (other.context_id_ == context_id_); 
  }

private:
  std::map<std::string, context::ContextValue> context_map_;

  int context_id_; 

  static int GetNextAvailableId(){
    return next_id_++; 
  }

  static std::atomic<int> next_id_;

};

std::atomic<int> Context::next_id_{0};
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
