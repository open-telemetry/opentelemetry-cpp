#include "opentelemetry/metrics_new/sync_instruments.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics_new
{

template <class T>
class NoopCounter : public opentelemetry::metrics_new::Counter
{
public:
  void Add(T value) noexcept override {}
  void Add(T value, const common::KeyValueIterable &attributes) {}
}

template <class T>
class NoopHistorgram : public opentelemetry::metrics_new::Historgram
{
public:
  void Add(T value) noexcept override {}
  void Add(T value, const common::KeyValueIterable &attributes) {}
}

template <class T>
class NoopUpDownCounter : public opentelemetry::metrics_new::UpDownCounter
{
public:
  void Add(T value) noexcept override {}
  void Add(T value, const common::KeyValueIterable &attributes) {}
}

template <class T>
class NoopObservableCounter : public ObservableCounter
{};

template <class T>
class NoopObservableGauge : public ObservableGauge
{};

template <class T>
class NoopObservableUpDownCounter : public ObservableUpDownCounter
{};

/**
 * No-op implementation of Meter.
 */
class NoopMeter final : public opentelemetry::Meter
{
public:
  // Tracer
  nostd::shared_ptr<Span> StartSpan(nostd::string_view /*name*/,
                                    const common::KeyValueIterable & /*attributes*/,
                                    const SpanContextKeyValueIterable & /*links*/,
                                    const StartSpanOptions & /*options*/) noexcept override
  {
    // Don't allocate a no-op span for every StartSpan call, but use a static
    // singleton for this case.
    static nostd::shared_ptr<trace_api::Span> noop_span(
        new trace_api::NoopSpan{this->shared_from_this()});

    return noop_span;
  }

  void ForceFlushWithMicroseconds(uint64_t /*timeout*/) noexcept override {}

  void CloseWithMicroseconds(uint64_t /*timeout*/) noexcept override {}
};

/**
 * No-op implementation of a TracerProvider.
 */
class NoopMeterProvider final : public opentelemetry::trace::MeterProvider
{
public:
  NoopMeterProvider()
      : tracer_{
            nostd::shared_ptr<opentelemetry::trace::NoopMeter>(new opentelemetry::trace::NoopMeter)}
  {}

  nostd::shared_ptr<opentelemetry::trace::Tracer> GetMeter(nostd::string_view library_name,
                                                           nostd::string_view library_version,
                                                           nostd::string_view schema_url) override
  {
    return meter_;
  }

private:
  nostd::shared_ptr<opentelemetry::trace::Tracer> meter_;
};
}  // namespace metrics_new
OPENTELEMETRY_END_NAMESPACE