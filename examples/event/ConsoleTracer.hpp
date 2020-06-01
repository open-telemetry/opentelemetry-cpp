#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <mutex>

#include <opentelemetry/trace/key_value_iterable_view.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/span_id.h>
#include <opentelemetry/trace/trace_id.h>
#include <opentelemetry/trace/tracer_provider.h>

#include <iomanip>
#include <iostream>
#include <sstream>

namespace core  = opentelemetry::core;
namespace trace = opentelemetry::trace;

OPENTELEMETRY_BEGIN_NAMESPACE

namespace console
{

std::string to_string(std::chrono::system_clock::time_point &tp)
{
  int64_t millis = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
  auto in_time_t = std::chrono::system_clock::to_time_t(tp);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
  ss << "." << std::setfill('0') << std::setw(3) << (unsigned)(millis % 1000);
  return ss.str();
}

class Span;

class Tracer : public trace::Tracer
{
  trace::TracerProvider &provider;

public:
  Tracer(trace::TracerProvider &parent) : trace::Tracer(), provider(parent) {}

  /**
   * TODO:
   * Is there a reason why we require unique_ptr? In most cases the lifetime of a span should be
   * tracked by the tracer, as every span has a parent tracer. Client code can hold a non-owning
   * reference to Span. There could be multiple clients obtaining a reference to the same Span
   * object. All clients must not use a Span if the Tracer is destroyed.
   */
  virtual nostd::unique_ptr<trace::Span> StartSpan(
      nostd::string_view name,
      const trace::StartSpanOptions &options = {}) noexcept override
  {
    return trace::to_span_ptr<Span, Tracer>(this, name, options);
  }

  virtual void ForceFlushWithMicroseconds(uint64_t timeout) noexcept override {}

  virtual void CloseWithMicroseconds(uint64_t timeout) noexcept override {}

  // TODO: pass span
  void LogEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                const trace::KeyValueIterable &attributes) noexcept
  {
    std::chrono::system_clock::time_point tp = timestamp;
    std::cout << to_string(tp);
    std::cout << '\t';
    std::cout << name;
    std::cout << '\t';

    attributes.ForEachKeyValue([&](nostd::string_view key, common::AttributeValue value) noexcept {
      std::cout << key << "=";
      std::cout << std::get<nostd::string_view>(value);
      std::cout << " ";
      return true;
    });

    std::cout << std::endl;

  };

  // TODO: pass span
  void LogEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept {}

  // TODO: pass span
  void LogEvent(nostd::string_view name) {}
};

class Span : public trace::Span
{

private:
  Tracer &owner;

public:
  Span(Tracer &owner, nostd::string_view name, const trace::StartSpanOptions &options) noexcept
      : trace::Span(), owner(owner)
  {
    (void)options;
  }

  ~Span() { End(); }

  void AddEvent(nostd::string_view name) noexcept { owner.LogEvent(name); }

  void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept
  {
    owner.LogEvent(name, timestamp);
  }

  void AddEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                const trace::KeyValueIterable &attributes) noexcept
  {
    owner.LogEvent(name, timestamp, attributes);
  }

  void SetStatus(trace::CanonicalCode code, nostd::string_view description) noexcept {}

  void UpdateName(nostd::string_view name) noexcept {}

  void End() noexcept {}

  bool IsRecording() const noexcept { return true; }

  trace::Tracer &tracer() const noexcept { return this->owner; };
};

class TracerProvider : public trace::TracerProvider
{
public:
  virtual nostd::shared_ptr<trace::Tracer> GetTracer(nostd::string_view library_name,
                                                     nostd::string_view library_version = "")
  {
    return nostd::shared_ptr<trace::Tracer>{new (std::nothrow) Tracer(*this)};
  }
};

}  // namespace console

OPENTELEMETRY_END_NAMESPACE
