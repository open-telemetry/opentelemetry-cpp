#pragma once

#include <iostream>
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/trace/key_value_iterable_view.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/version.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
class Link final
{
public:
  Link() = default;

  Link(const opentelemetry::trace::SpanContext &span_context,
       const trace_api::KeyValueIterable &attributes)
      : span_context_(span_context), attributes_{attributes}
  {}

  template <class T, nostd::enable_if_t<detail::is_key_value_iterable<T>::value> * = nullptr>
  Link(const SpanContext &span_context, const T &attributes)
      : Link(span_context, KeyValueIterableView<T>(attributes))
  {}

  Link(const SpanContext &span_context,
       std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>> attributes)
      : Link(span_context,
             nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{
                 attributes.begin(), attributes.end()})
  {}

  Link(const opentelemetry::trace::SpanContext &span_context)
      : Link(span_context,
             nostd::span<const std::pair<nostd::string_view, common::AttributeValue>>{})
  {}

  // returns the Span Context associated with this Link
  const SpanContext &GetSpanContext() const { return span_context_; }

  // returns the attributes associated with link
  const KeyValueIterable &GetAttributes() const { return attributes_; }

private:
  const trace_api::SpanContext &span_context_;
  const trace_api::KeyValueIterable &attributes_;
};
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE
