#pragma once

#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/trace/key_value_iterable_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{
/**
 * No-op implementation of Meter. This class should not be used directly.
 */
class NoopMeter : public Meter
{
public:
  NoopMeter() = default;

  opentelemetry::nostd::shared_ptr<SynchronousInstrument> NewDoubleCounter(nostd::string_view name,
                                                                   nostd::string_view description,
                                                                   nostd::string_view unit,
                                                                   const bool enabled) override
  {
    return nostd::shared_ptr<SynchronousInstrument>{
        new NoopDoubleCounter(name, description, unit, enabled)};
  }

  opentelemetry::nostd::shared_ptr<SynchronousInstrument> NewIntCounter(nostd::string_view name,
                                                             nostd::string_view description,
                                                             nostd::string_view unit,
                                                             const bool enabled) override
  {
    return nostd::shared_ptr<SynchronousInstrument>{new NoopIntCounter(name, description, unit, enabled)};
  }

  opentelemetry::nostd::shared_ptr<SynchronousInstrument> NewDoubleUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<SynchronousInstrument>{
        new NoopDoubleUpDownCounter(name, description, unit, enabled)};
  }

  opentelemetry::nostd::shared_ptr<SynchronousInstrument> NewIntUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<SynchronousInstrument>{
        new NoopIntUpDownCounter(name, description, unit, enabled)};
  }

  opentelemetry::nostd::shared_ptr<SynchronousInstrument> NewDoubleValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<SynchronousInstrument>{
        new NoopDoubleValueRecorder(name, description, unit, enabled)};
  }

  opentelemetry::nostd::shared_ptr<SynchronousInstrument> NewIntValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<SynchronousInstrument>{
        new NoopIntValueRecorder(name, description, unit, enabled)};
  }

  opentelemetry::nostd::shared_ptr<AsynchronousInstrument> NewDoubleSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(DoubleObserverResult)) override
  {
    return nostd::shared_ptr<AsynchronousInstrument>{
        new NoopDoubleSumObserver(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::shared_ptr<AsynchronousInstrument> NewIntSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(IntObserverResult)) override
  {
    return nostd::shared_ptr<AsynchronousInstrument>{
        new NoopIntSumObserver(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::shared_ptr<AsynchronousInstrument> NewDoubleUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(DoubleObserverResult)) override
  {
    return nostd::shared_ptr<AsynchronousInstrument>{
        new NoopDoubleUpDownSumObserver(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::shared_ptr<AsynchronousInstrument> NewIntUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(IntObserverResult)) override
  {
    return nostd::shared_ptr<AsynchronousInstrument>{
        new NoopIntUpDownSumObserver(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::shared_ptr<AsynchronousInstrument> NewDoubleValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(DoubleObserverResult)) override
  {
    return nostd::shared_ptr<AsynchronousInstrument>{
        new NoopDoubleValueObserver(name, description, unit, enabled, callback)};
  }

  void RecordBatch(nostd::string_view /*labels*/,
                   const trace::KeyValueIterable & /*values*/) noexcept override
  {
    // No-op
  }
};
}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
