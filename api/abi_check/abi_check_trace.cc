
#include "opentelemetry/trace/canonical_code.h"
#include "opentelemetry/trace/propagation/b3_propagator.h"
#include "opentelemetry/trace/propagation/detail/hex.h"
#include "opentelemetry/trace/propagation/detail/string.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/trace/propagation/jaeger.h"
#include "opentelemetry/trace/scope.h"
#include "opentelemetry/trace/span_context.h"
#include "opentelemetry/trace/span_context_kv_iterable_view.h"
#include "opentelemetry/trace/trace_flags.h"
#include "opentelemetry/trace/trace_id.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/trace/context.h"
#include "opentelemetry/trace/span_metadata.h"
#include "opentelemetry/trace/span_id.h"
#include "opentelemetry/trace/noop.h"
#include "opentelemetry/trace/trace_state.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/default_span.h"
#include "opentelemetry/trace/span_context_kv_iterable.h"
#include "opentelemetry/trace/span_startoptions.h"
#include "opentelemetry/trace/semantic_conventions.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/trace/tracer_provider.h"

void do_abi_check_trace() {
  auto p = opentelemetry::trace::Provider::GetTracerProvider();
  auto tracer = p->GetTracer("abi");
  auto span = tracer->StartSpan("abi");
  span->End();
}

