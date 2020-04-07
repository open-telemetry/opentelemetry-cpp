#pragma once

#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/trace/attribute_value.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
class KeyValueIterable
{
public:
  virtual ~KeyValueIterable() = default;

  virtual bool ForEachKeyValue(
      nostd::function_ref<bool(nostd::string_view, AttributeValue)> callback) const noexcept = 0;

  virtual size_t size() const noexcept = 0;
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
