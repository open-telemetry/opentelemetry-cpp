// Need to include everything needed to get #include <opentelemetry_API exports

#include <opentelemetry/baggage/baggage.h>
#include <opentelemetry/baggage/baggage_context.h>
#include <opentelemetry/baggage/propagation/baggage_propagator.h>
#include <opentelemetry/common/attribute_value.h>
#include <opentelemetry/common/key_value_iterable.h>
#include <opentelemetry/common/key_value_iterable_view.h>
#include <opentelemetry/common/kv_properties.h>
#include <opentelemetry/common/macros.h>
#include <opentelemetry/common/spin_lock_mutex.h>
#include <opentelemetry/common/string_util.h>
#include <opentelemetry/common/timestamp.h>
#include <opentelemetry/context/context.h>
#include <opentelemetry/context/context_value.h>
#include <opentelemetry/context/runtime_context.h>
#include <opentelemetry/context/propagation/composite_propagator.h>
#include <opentelemetry/context/propagation/global_propagator.h>
#include <opentelemetry/context/propagation/noop_propagator.h>
#include <opentelemetry/context/propagation/text_map_propagator.h>
#include <opentelemetry/logs/logger.h>
#include <opentelemetry/logs/logger_provider.h>
#include <opentelemetry/logs/log_record.h>
#include <opentelemetry/logs/noop.h>
#include <opentelemetry/logs/provider.h>
#include <opentelemetry/logs/severity.h>
#include <opentelemetry/metrics/async_instruments.h>
#include <opentelemetry/metrics/meter.h>
#include <opentelemetry/metrics/meter_provider.h>
#include <opentelemetry/metrics/noop.h>
#include <opentelemetry/metrics/observer_result.h>
#include <opentelemetry/metrics/provider.h>
#include <opentelemetry/metrics/sync_instruments.h>
#include <opentelemetry/plugin/dynamic_load.h>
#include <opentelemetry/plugin/factory.h>
#include <opentelemetry/plugin/hook.h>
#include <opentelemetry/plugin/tracer.h>
#include <opentelemetry/std/shared_ptr.h>
#include <opentelemetry/std/span.h>
#include <opentelemetry/std/string_view.h>
#include <opentelemetry/std/type_traits.h>
#include <opentelemetry/std/unique_ptr.h>
#include <opentelemetry/std/utility.h>
#include <opentelemetry/std/variant.h>
#include <opentelemetry/trace/canonical_code.h>
#include <opentelemetry/trace/context.h>
#include <opentelemetry/trace/default_span.h>
#include <opentelemetry/trace/noop.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/trace/scope.h>
#include <opentelemetry/trace/semantic_conventions.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/span_context.h>
#include <opentelemetry/trace/span_context_kv_iterable.h>
#include <opentelemetry/trace/span_context_kv_iterable_view.h>
#include <opentelemetry/trace/span_id.h>
#include <opentelemetry/trace/span_metadata.h>
#include <opentelemetry/trace/span_startoptions.h>
#include <opentelemetry/trace/tracer.h>
#include <opentelemetry/trace/tracer_provider.h>
#include <opentelemetry/trace/trace_flags.h>
#include <opentelemetry/trace/trace_id.h>
#include <opentelemetry/trace/trace_state.h>
#include <opentelemetry/trace/propagation/b3_propagator.h>
#include <opentelemetry/trace/propagation/http_trace_context.h>
#include <opentelemetry/trace/propagation/jaeger.h>

// This hack forces static inline functions, marked with OPENTELEMETRY_API_SINGLETON to be linked in.
// It goes by either directly adding them to this list, or adding something that calls them (if they are private).
extern "C" OPENTELEMETRY_API void** return_something()
{
static void* otel_sdk_force_link[] = {
    &opentelemetry::baggage::Baggage::GetDefault,
    &opentelemetry::baggage::Baggage::FromHeader,
    &opentelemetry::context::GetDefaultStorage,
    &opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator,
    &opentelemetry::context::propagation::GlobalTextMapPropagator::SetGlobalPropagator,
    &opentelemetry::context::RuntimeContext::Attach,
    &opentelemetry::context::RuntimeContext::Detach,
    &opentelemetry::context::RuntimeContext::GetConstRuntimeContextStorage,
    &opentelemetry::context::RuntimeContext::GetConstRuntimeContextStorage,
    &opentelemetry::context::RuntimeContext::GetCurrent,
    &opentelemetry::context::RuntimeContext::GetValue,
    &opentelemetry::context::RuntimeContext::SetRuntimeContextStorage,
    &opentelemetry::context::RuntimeContext::SetValue,
    &opentelemetry::metrics::Provider::GetMeterProvider,
    &opentelemetry::metrics::Provider::SetMeterProvider,
    &opentelemetry::trace::Provider::GetTracerProvider,
    &opentelemetry::trace::Provider::SetTracerProvider,
    &opentelemetry::trace::TraceState::GetDefault,
    &opentelemetry::trace::TraceState::FromHeader,
    &opentelemetry::trace::TraceState::IsValidKey,
    &opentelemetry::trace::TraceState::IsValidValue,
    nullptr
};
return otel_sdk_force_link;
}
