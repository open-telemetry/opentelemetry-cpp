#pragma once

#include "opentelemetry/trace/link.h"
#include "opentelemetry/sdk/trace/attribute_utils.h"
#include "opentelemetry/version.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace trace
{
class Link final : public trace_api::Link
{
public:
    Link( opentelemetry::trace::SpanContext span_context, 
          const opentelemetry::trace::KeyValueIterableView<std::unordered_map<nostd::string_view, opentelemetry::common::AttributeValue>> &attributes) noexcept
        : span_context_(span_context), attribute_map_{attributes}
    {
    }
 
    const trace_api::SpanContext& GetContext() const noexcept override
    {
      return span_context_;
    }

    const trace_api::KeyValueIterable & GetAttributes() const noexcept override
    {
      return attribute_map_;
      
    }

private:
  trace_api::SpanContext span_context_;
  const trace_api::KeyValueIterableView<std::unordered_map<nostd::string_view, opentelemetry::common::AttributeValue>> attribute_map_;
};
}  // namespace trace
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
