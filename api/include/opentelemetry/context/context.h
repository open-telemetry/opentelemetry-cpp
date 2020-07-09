#pragma once

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/trace/key_value_iterable_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

// The context class provides a context identifier.
// This is a dummy class that is meant to be overridden,
// the methods return default values.
class Context
{

public:
  // The Key class is used to obscure access from the
  //  user to the context map. The identifier is used as a key
  // to the context map.
  class Key
  {

  public:
    // Returns the key's identifier.
    nostd::string_view GetIdentifier() { return ""; }

    // Returns the key's name
    nostd::string_view GetName() { return ""; }

    Key() = default;

    // Constructs a new Key with the passed in name. Sets the identifier as
    // the address of this object.
    Key(nostd::string_view key_name) {}

  private:
    friend class Context;
  };

  // Creates a key with the passed in name and returns it.
  Key CreateKey(nostd::string_view key_name) { return Key(key_name); }

  Context() = default;

  // Contructor, creates a context object from a map of keys
  // and identifiers.
  template <class T, nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
  Context(const T values)
  {}

  // Accepts a new iterable and then returns a new context that
  // contains both the original pairs and the new pair.
  template <class T, nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
  Context WriteValues(T &values) noexcept
  {
    return Context(values);
  }

  // Returns the value associated with the passed in key.
  virtual common::AttributeValue GetValue(Key key) { return 0; }

  // Copy Constructors.
  Context(const Context &other) = default;
  Context &operator=(const Context &other) = default;
};

}  // namespace context
OPENTELEMETRY_END_NAMESPACE
