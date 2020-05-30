#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <opentelemetry/trace/key_value_iterable_view.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/span_id.h>
#include <opentelemetry/trace/trace_id.h>
#include <opentelemetry/trace/tracer_provider.h>

using namespace OPENTELEMETRY_NAMESPACE;

using namespace opentelemetry::trace;

class ConsoleTracer : public Tracer
{
  TracerProvider &provider;

public:
  ConsoleTracer(TracerProvider &parent) : provider(parent) {}

  virtual nostd::unique_ptr<Span> StartSpan(nostd::string_view name, const StartSpanOptions &options = {}) noexcept
  {}

  virtual void ForceFlushWithMicroseconds(uint64_t timeout) noexcept {}

  virtual void CloseWithMicroseconds(uint64_t timeout) noexcept {}
};

class ConsoleTracerProvider : public TracerProvider
{
public:
  virtual nostd::shared_ptr<Tracer> GetTracer(nostd::string_view library_name, nostd::string_view library_version = "")
  {
    return nullptr;
  }
};
