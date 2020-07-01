#pragma once

#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/trace/key_value_iterable_view.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace meter
{
/**
 * No-op implementation of Meter. This class should not be used directly.
 */
class NoopMeter : public meter::Meter
{
public:
  NoopMeter() = default;

  opentelemetry::nostd::unique_ptr<metrics::DoubleCounter> NewDoubleCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::unique_ptr<metrics::DoubleCounter>{
        new NoopDoubleCounter(name, description, unit, enabled)};
  }

  opentelemetry::nostd::unique_ptr<metrics::IntCounter> NewIntCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::unique_ptr<metrics::IntCounter>{
        new NoopIntCounter(name, description, unit, enabled)};
  }

  opentelemetry::nostd::unique_ptr<metrics::DoubleUpDownCounter> NewDoubleUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::unique_ptr<metrics::DoubleUpDownCounter>{
        new NoopDoubleUpDownCounter(name, description, unit, enabled)};
  }

  opentelemetry::nostd::unique_ptr<metrics::IntUpDownCounter> NewIntUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::unique_ptr<metrics::IntUpDownCounter>{
        new NoopIntUpDownCounter(name, description, unit, enabled)};
  }

  opentelemetry::nostd::unique_ptr<metrics::DoubleValueRecorder> NewDoubleValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::unique_ptr<metrics::DoubleValueRecorder>{
        new NoopDoubleValueRecorder(name, description, unit, enabled)};
  }

  opentelemetry::nostd::unique_ptr<metrics::IntValueRecorder> NewIntValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::unique_ptr<metrics::IntValueRecorder>{
        new NoopIntValueRecorder(name, description, unit, enabled)};
  }

  /*
  opentelemetry::nostd::unique_ptr<metrics::DoubleSumObserver> NewDoubleSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::unique_ptr<metrics::DoubleSumObserver>{new NoopDoubleSumObserver(name,
  description, unit, enabled)};
  }
  opentelemetry::nostd::unique_ptr<metrics::IntSumObserver> NewIntSumObserver(nostd::string_view
  name, nostd::string_view description, nostd::string_view unit, const bool enabled) override
  {
    return nostd::unique_ptr<metrics::IntSumObserver>{new NoopIntSumObserver(name, description,
  unit, enabled)};
  }
  opentelemetry::nostd::unique_ptr<metrics::DoubleUpDownSumObserver> NewDoubleUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::unique_ptr<metrics::DoubleUpDownSumObserver>{
        new NoopDoubleUpDownSumObserver(name, description, unit, enabled)};
  }
  opentelemetry::nostd::unique_ptr<metrics::IntUpDownSumObserver> NewIntUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::unique_ptr<metrics::IntUpDownSumObserver>{
        new NoopIntUpDownSumObserver(name, description, unit, enabled)};
  }
  opentelemetry::nostd::unique_ptr<metrics::DoubleValueObserver> NewDoubleValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::unique_ptr<metrics::DoubleValueObserver>{new NoopDoubleValueObserver(name,
  description, unit, enabled)};
  }
  */

private:
  void RecordBatch(nostd::string_view labels,
                   const trace::KeyValueIterable &values) noexcept override
  {
    // No-op
  }

};

}  // namespace meter
OPENTELEMETRY_END_NAMESPACE
