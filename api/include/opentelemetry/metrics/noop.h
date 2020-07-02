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

  opentelemetry::nostd::unique_ptr<DoubleCounter> NewDoubleCounter(nostd::string_view name,
                                                                   nostd::string_view description,
                                                                   nostd::string_view unit,
                                                                   const bool enabled) override
  {
    return nostd::unique_ptr<DoubleCounter>{
        new NoopDoubleCounter(name, description, unit, enabled)};
  }

  opentelemetry::nostd::unique_ptr<IntCounter> NewIntCounter(nostd::string_view name,
                                                             nostd::string_view description,
                                                             nostd::string_view unit,
                                                             const bool enabled) override
  {
    return nostd::unique_ptr<IntCounter>{new NoopIntCounter(name, description, unit, enabled)};
  }

  opentelemetry::nostd::unique_ptr<DoubleUpDownCounter> NewDoubleUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::unique_ptr<DoubleUpDownCounter>{
        new NoopDoubleUpDownCounter(name, description, unit, enabled)};
  }

  opentelemetry::nostd::unique_ptr<IntUpDownCounter> NewIntUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::unique_ptr<IntUpDownCounter>{
        new NoopIntUpDownCounter(name, description, unit, enabled)};
  }

  opentelemetry::nostd::unique_ptr<DoubleValueRecorder> NewDoubleValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::unique_ptr<DoubleValueRecorder>{
        new NoopDoubleValueRecorder(name, description, unit, enabled)};
  }

  opentelemetry::nostd::unique_ptr<IntValueRecorder> NewIntValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::unique_ptr<IntValueRecorder>{
        new NoopIntValueRecorder(name, description, unit, enabled)};
  }

  opentelemetry::nostd::unique_ptr<DoubleSumObserver> NewDoubleSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(DoubleObserverResult)) override
  {
    return nostd::unique_ptr<DoubleSumObserver>{
        new NoopDoubleSumObserver(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::unique_ptr<IntSumObserver> NewIntSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(IntObserverResult)) override
  {
    return nostd::unique_ptr<IntSumObserver>{
        new NoopIntSumObserver(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::unique_ptr<DoubleUpDownSumObserver> NewDoubleUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(DoubleObserverResult)) override
  {
    return nostd::unique_ptr<DoubleUpDownSumObserver>{
        new NoopDoubleUpDownSumObserver(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::unique_ptr<IntUpDownSumObserver> NewIntUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(IntObserverResult)) override
  {
    return nostd::unique_ptr<IntUpDownSumObserver>{
        new NoopIntUpDownSumObserver(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::unique_ptr<DoubleValueObserver> NewDoubleValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(DoubleObserverResult)) override
  {
    return nostd::unique_ptr<DoubleValueObserver>{
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
