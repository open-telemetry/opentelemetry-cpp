#pragma once

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace common
{
/**
 * Supports internal iteration over a collection of key-value pairs.
 */
class KeyValueIterable
{
public:
  virtual ~KeyValueIterable() = default;

  /**
   * Iterate over key-value pairs
   * @param callback a callback to invoke for each key-value. If the callback returns false,
   * the iteration is aborted.
   * @return true if every key-value pair was iterated over
   */
  virtual bool ForEachKeyValue(nostd::function_ref<bool(nostd::string_view, common::AttributeValue)>
                                   callback) const noexcept = 0;

  /**
   * @return the number of key-value pairs
   */
  virtual size_t size() const noexcept = 0;
};

//
// NULL object pattern empty iterable.
//
class NullKeyValueIterable : public KeyValueIterable
{
public:
  NullKeyValueIterable(){};

  virtual bool ForEachKeyValue(
      nostd::function_ref<bool(nostd::string_view, common::AttributeValue)>) const noexcept
  {
    return true;
  };

  virtual size_t size() const noexcept { return 0; }
};

}  // namespace common
OPENTELEMETRY_END_NAMESPACE
