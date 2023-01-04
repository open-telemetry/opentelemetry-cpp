/*
 * Copyright The OpenTelemetry Authors
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tracer_shim.h"
#include "span_shim.h"
#include "shim_utils.h"

#include <opentelemetry/baggage/baggage_context.h>
#include <opentelemetry/context/propagation/global_propagator.h>
#include <opentelemetry/trace/context.h>
#include <opentelemetry/trace/semantic_conventions.h>
#include <opentracing/ext/tags.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace opentracingshim
{

namespace detail
{

using RefsList = std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>;
using LinksList = std::vector<std::pair<opentelemetry::trace::SpanContext, RefsList>>;

static opentelemetry::trace::StartSpanOptions makeOptionsShim(const opentracing::StartSpanOptions& options) noexcept
{
  using opentracing::SpanReferenceType;
  // If an explicit start timestamp is specified, a conversion MUST
  // be done to match the OpenTracing and OpenTelemetry units.
  opentelemetry::trace::StartSpanOptions options_shim;
  options_shim.start_system_time = opentelemetry::common::SystemTimestamp{options.start_system_timestamp};
  options_shim.start_steady_time = opentelemetry::common::SteadyTimestamp{options.start_steady_timestamp};

  const auto& refs = options.references;
  // If a list of Span references is specified...
  if (!refs.empty())
  {
    auto first_child_of = std::find_if(refs.cbegin(), refs.cend(),
      [](const std::pair<SpanReferenceType, const opentracing::SpanContext*>& entry){
        return entry.first == SpanReferenceType::ChildOfRef;
      });
    // The first SpanContext with Child Of type in the entire list is used as parent,
    // else the first SpanContext is used as parent
    auto context = (first_child_of != refs.cend()) ? first_child_of->second : refs.cbegin()->second;

    if (auto context_shim = dynamic_cast<const SpanContextShim*>(context))
    {
      options_shim.parent = context_shim->context();
    }
  }

  return options_shim;
}

static LinksList makeReferenceLinks(const opentracing::StartSpanOptions& options) noexcept
{
  using opentracing::SpanReferenceType;
  using namespace opentelemetry::trace::SemanticConventions;

  LinksList links;
  links.reserve(options.references.size());
  // All values in the list MUST be added as Links with the reference type value
  // as a Link attribute, i.e. opentracing.ref_type set to follows_from or child_of
  for (const auto& entry : options.references)
  {
    auto context_shim = dynamic_cast<const SpanContextShim*>(entry.second);
    nostd::string_view span_kind;

    if (entry.first == SpanReferenceType::ChildOfRef)
    {
      span_kind = OpentracingRefTypeValues::kChildOf;
    }
    else if (entry.first == SpanReferenceType::FollowsFromRef)
    {
      span_kind = OpentracingRefTypeValues::kFollowsFrom;
    }

    if (context_shim && !span_kind.empty())
    {
      // links.push_back({ context_shim->context(), {{ opentracing::ext::span_kind.data(), span_kind }} });
      links.emplace_back(std::piecewise_construct,
                         std::forward_as_tuple(context_shim->context()),
                         std::forward_as_tuple(std::forward<RefsList>({{ opentracing::ext::span_kind.data(), span_kind }})));
    }
  }

  return links;
}

static BaggagePtr makeBaggage(const opentracing::StartSpanOptions& options) noexcept
{
  using namespace opentelemetry::baggage;

  std::unordered_map<std::string, std::string> baggage_items;
  // If a list of Span references is specified...
  for (const auto& entry : options.references)
  {
    if (auto context_shim = dynamic_cast<const SpanContextShim*>(entry.second))
    {
      // The union of their Baggage values MUST be used as the initial Baggage of the newly created Span.
      context_shim->ForeachBaggageItem([&baggage_items](const std::string& key, const std::string& value){
        // It is unspecified which Baggage value is used in the case of repeated keys.
        if (baggage_items.find(key) == baggage_items.end())
        {
          baggage_items.emplace(key, value); // Here, only insert if key not already present
        }
        return true;
      });
    }
  }
  // If no such lisf of references is specified, the current Baggage
  // MUST be used as the initial value of the newly created Span.
  return baggage_items.empty()
    ? GetBaggage(opentelemetry::context::RuntimeContext::GetCurrent())
    : nostd::shared_ptr<Baggage>(new Baggage(baggage_items));
}

static std::vector<std::pair<std::string, common::AttributeValue>> makeTags(const opentracing::StartSpanOptions& options) noexcept
{
  std::vector<std::pair<std::string, common::AttributeValue>> tags;
  tags.reserve(options.tags.size());

  // If an initial set of tags is specified, the values MUST
  // be set at the creation time of the OpenTelemetry Span.
  for (const auto& entry : options.tags)
  {
    tags.emplace_back(entry.first, utils::attributeFromValue(entry.second));
  }

  return tags;
}

} // namespace opentracingshim::detail

std::unique_ptr<opentracing::Span> TracerShim::StartSpanWithOptions(opentracing::string_view operation_name,
                                                                    const opentracing::StartSpanOptions& options) const noexcept
{
  if (is_closed_) return nullptr;

  const auto& opts = detail::makeOptionsShim(options);
  const auto& links = detail::makeReferenceLinks(options);
  const auto& baggage = detail::makeBaggage(options);
  const auto& attributes = detail::makeTags(options);
  auto span = tracer_->StartSpan(operation_name.data(), attributes, links, opts);
  auto span_shim = new SpanShim(*this, span, baggage);

  // If an initial set of tags is specified and the OpenTracing error tag
  // is included after the OpenTelemetry Span was created.
  const auto& error_entry = std::find_if(options.tags.begin(), options.tags.end(),
    [](const std::pair<std::string, opentracing::v3::Value>& entry){
      return entry.first == opentracing::ext::error;
    });
  // The Shim layer MUST perform the same error handling as described in the Set Tag operation
  if (error_entry != options.tags.end()) {
    span_shim->handleError(error_entry->second);
  }

  return std::unique_ptr<opentracing::Span>(span_shim);
}

opentracing::expected<void> TracerShim::Inject(const opentracing::SpanContext& sc,
                                               std::ostream& writer) const
{
  // Errors MAY be raised if the specified Format is not recognized,
  // depending on the specific OpenTracing Language API.
  return opentracing::make_unexpected(opentracing::invalid_carrier_error);
}

opentracing::expected<void> TracerShim::Inject(const opentracing::SpanContext& sc,
                                               const opentracing::TextMapWriter& writer) const
{
  // TextMap and HttpHeaders formats MUST use their explicitly specified TextMapPropagator,
  // if any, or else use the global TextMapPropagator.
  const auto& propagator = propagators_.text_map
    ? propagators_.text_map
    : opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();

  return injectImpl(sc, writer, propagator);
}

opentracing::expected<void> TracerShim::Inject(const opentracing::SpanContext& sc,
                                               const opentracing::HTTPHeadersWriter& writer) const
{
  // TextMap and HttpHeaders formats MUST use their explicitly specified TextMapPropagator,
  // if any, or else use the global TextMapPropagator.
  const auto& propagator = propagators_.http_headers
    ? propagators_.http_headers
    : opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();

  return injectImpl(sc, writer, propagator);
}

opentracing::expected<std::unique_ptr<opentracing::SpanContext>> TracerShim::Extract(std::istream& reader) const
{
  // Errors MAY be raised if either the Format is not recognized or no value
  // could be extracted, depending on the specific OpenTracing Language API.
  return opentracing::make_unexpected(opentracing::invalid_carrier_error);
}

opentracing::expected<std::unique_ptr<opentracing::SpanContext>> TracerShim::Extract(const opentracing::TextMapReader& reader) const
{
  // TextMap and HttpHeaders formats MUST use their explicitly specified TextMapPropagator,
  // if any, or else use the global TextMapPropagator.
  const auto& propagator = propagators_.text_map
    ? propagators_.text_map
    : opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();

  return extractImpl(reader, propagator);
}

opentracing::expected<std::unique_ptr<opentracing::SpanContext>> TracerShim::Extract(const opentracing::HTTPHeadersReader& reader) const
{
  // TextMap and HttpHeaders formats MUST use their explicitly specified TextMapPropagator,
  // if any, or else use the global TextMapPropagator.
  const auto& propagator = propagators_.http_headers
    ? propagators_.http_headers
    : opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();

  return extractImpl(reader, propagator);
}

template <typename T>
opentracing::expected<void> TracerShim::injectImpl(const opentracing::SpanContext& sc,
                                                   const T& writer,
                                                   const PropagatorPtr& propagator) const
{
  // Inject the underlying OpenTelemetry Span and Baggage using either the explicitly registered
  // or the global OpenTelemetry Propagators, as configured at construction time.
  if (auto context_shim = dynamic_cast<const SpanContextShim*>(&sc))
  {
    auto current_context = opentelemetry::context::RuntimeContext::GetCurrent();
    // It MUST inject any non-empty Baggage even amidst no valid SpanContext.
    const auto& context = opentelemetry::baggage::SetBaggage(current_context, context_shim->baggage());

    utils::CarrierWriterShim<T> carrier{writer};
    propagator->Inject(carrier, context);
  }

  return opentracing::make_expected();
}

template <typename T>
opentracing::expected<std::unique_ptr<opentracing::SpanContext>> TracerShim::extractImpl(const T& reader,
                                                                                         const PropagatorPtr& propagator) const
{
  // Extract the underlying OpenTelemetry Span and Baggage using either the explicitly registered
  // or the global OpenTelemetry Propagators, as configured at construction time.
  utils::CarrierReaderShim<T> carrier{reader};
  auto current_context = opentelemetry::context::RuntimeContext::GetCurrent();
  auto context = propagator->Extract(carrier, current_context);
  auto span_context = opentelemetry::trace::GetSpan(context)->GetContext();
  auto baggage = opentelemetry::baggage::GetBaggage(context);

  // If the extracted SpanContext is invalid AND the extracted Baggage is empty,
  // this operation MUST return a null value, and otherwise it MUST return a
  // SpanContext Shim instance with the extracted values.
  SpanContextShim* context_shim = (!span_context.IsValid() && utils::isBaggageEmpty(baggage))
    ? nullptr
    : new SpanContextShim(span_context, baggage);

  return opentracing::make_expected(std::unique_ptr<opentracing::SpanContext>(context_shim));
}

} // namespace opentracingshim
OPENTELEMETRY_END_NAMESPACE