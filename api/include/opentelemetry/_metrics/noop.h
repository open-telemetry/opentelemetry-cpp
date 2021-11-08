// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW
// Please refer to provider.h for documentation on how to obtain a Meter object.
//
// This file is part of the internal implementation of OpenTelemetry. Nothing in this file should be
// used directly. Please refer to meter.h for documentation on these interfaces.

#  include "opentelemetry/_metrics/async_instruments.h"
#  include "opentelemetry/_metrics/instrument.h"
#  include "opentelemetry/_metrics/meter.h"
#  include "opentelemetry/_metrics/meter_provider.h"
#  include "opentelemetry/_metrics/sync_instruments.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/nostd/unique_ptr.h"
#  include "opentelemetry/version.h"

#  include <memory>

// TODO: Create generic short pattern for opentelemetry::common and opentelemetry::trace and others
// as necessary

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{
template <class T>
class NoopValueObserver : public ValueObserver<T>
{

public:
  NoopValueObserver(nostd::string_view /*name*/,
                    nostd::string_view /*description*/,
                    nostd::string_view /*unit*/,
                    bool /*enabled*/,
                    void (*callback)(ObserverResult<T>))
  {}

  virtual bool IsEnabled() override { return false; }

  virtual nostd::string_view GetName() override { return nostd::string_view(""); }

  virtual nostd::string_view GetDescription() override { return nostd::string_view(""); }

  virtual nostd::string_view GetUnits() override { return nostd::string_view(""); }

  virtual void observe(T value, const common::KeyValueIterable &labels) override {}

  virtual void run() override {}

  virtual InstrumentKind GetKind() override { return InstrumentKind::Counter; }
};

template <class T>
class NoopSumObserver : public SumObserver<T>
{

public:
  NoopSumObserver(nostd::string_view /*name*/,
                  nostd::string_view /*description*/,
                  nostd::string_view /*unit*/,
                  bool /*enabled*/,
                  void (*callback)(ObserverResult<T>))
  {}

  virtual bool IsEnabled() override { return false; }

  virtual nostd::string_view GetName() override { return nostd::string_view(""); }

  virtual nostd::string_view GetDescription() override { return nostd::string_view(""); }

  virtual nostd::string_view GetUnits() override { return nostd::string_view(""); }

  virtual void observe(T value, const common::KeyValueIterable &labels) override {}

  virtual void run() override {}

  virtual InstrumentKind GetKind() override { return InstrumentKind::Counter; }
};

template <class T>
class NoopUpDownSumObserver : public UpDownSumObserver<T>
{

public:
  NoopUpDownSumObserver(nostd::string_view /*name*/,
                        nostd::string_view /*description*/,
                        nostd::string_view /*unit*/,
                        bool /*enabled*/,
                        void (*callback)(ObserverResult<T>))
  {}

  virtual bool IsEnabled() override { return false; }

  virtual nostd::string_view GetName() override { return nostd::string_view(""); }

  virtual nostd::string_view GetDescription() override { return nostd::string_view(""); }

  virtual nostd::string_view GetUnits() override { return nostd::string_view(""); }

  virtual void observe(T value, const common::KeyValueIterable &labels) override {}

  virtual void run() override {}

  virtual InstrumentKind GetKind() override { return InstrumentKind::Counter; }
};

template <class T>
class BoundNoopCounter : public BoundCounter<T>
{

public:
  BoundNoopCounter() = default;

  BoundNoopCounter(nostd::string_view /*name*/,
                   nostd::string_view /*description*/,
                   nostd::string_view /*unit*/,
                   bool /*enabled*/)
  {}

  virtual void add(T value) override {}

  virtual bool IsEnabled() override { return false; }

  virtual nostd::string_view GetName() override { return nostd::string_view(""); }

  virtual nostd::string_view GetDescription() override { return nostd::string_view(""); }

  virtual nostd::string_view GetUnits() override { return nostd::string_view(""); }

  virtual InstrumentKind GetKind() override { return InstrumentKind::Counter; }
};

template <class T>
class NoopCounter : public Counter<T>
{

public:
  NoopCounter() = default;

  NoopCounter(nostd::string_view /*name*/,
              nostd::string_view /*description*/,
              nostd::string_view /*unit*/,
              bool /*enabled*/)
  {}

  nostd::shared_ptr<BoundNoopCounter<T>> bindNoopCounter(
      const common::KeyValueIterable & /*labels*/)
  {
    return nostd::shared_ptr<BoundNoopCounter<T>>(new BoundNoopCounter<T>());
  }

  virtual void add(T value, const common::KeyValueIterable & /*labels*/) override {}

  virtual void update(T value, const common::KeyValueIterable & /*labels*/) override {}

  virtual bool IsEnabled() override { return false; }

  virtual nostd::string_view GetName() override { return nostd::string_view(""); }

  virtual nostd::string_view GetDescription() override { return nostd::string_view(""); }

  virtual nostd::string_view GetUnits() override { return nostd::string_view(""); }

  virtual InstrumentKind GetKind() override { return InstrumentKind::Counter; }
};

template <class T>
class BoundNoopUpDownCounter : public BoundUpDownCounter<T>
{

public:
  BoundNoopUpDownCounter() = default;

  BoundNoopUpDownCounter(nostd::string_view /*name*/,
                         nostd::string_view /*description*/,
                         nostd::string_view /*unit*/,
                         bool /*enabled*/)
  {}

  virtual void add(T value) override {}

  virtual bool IsEnabled() override { return false; }

  virtual nostd::string_view GetName() override { return nostd::string_view(""); }

  virtual nostd::string_view GetDescription() override { return nostd::string_view(""); }

  virtual nostd::string_view GetUnits() override { return nostd::string_view(""); }

  virtual InstrumentKind GetKind() override { return InstrumentKind::UpDownCounter; }
};

template <class T>
class NoopUpDownCounter : public UpDownCounter<T>
{

public:
  NoopUpDownCounter() = default;

  NoopUpDownCounter(nostd::string_view /*name*/,
                    nostd::string_view /*description*/,
                    nostd::string_view /*unit*/,
                    bool /*enabled*/)
  {}

  nostd::shared_ptr<BoundNoopUpDownCounter<T>> bindNoopUpDownCounter(
      const common::KeyValueIterable & /*labels*/)
  {
    return nostd::shared_ptr<BoundNoopUpDownCounter<T>>(new BoundNoopUpDownCounter<T>());
  }

  virtual void add(T value, const common::KeyValueIterable & /*labels*/) override {}

  virtual void update(T value, const common::KeyValueIterable & /*labels*/) override {}

  virtual bool IsEnabled() override { return false; }

  virtual nostd::string_view GetName() override { return nostd::string_view(""); }

  virtual nostd::string_view GetDescription() override { return nostd::string_view(""); }

  virtual nostd::string_view GetUnits() override { return nostd::string_view(""); }

  virtual InstrumentKind GetKind() override { return InstrumentKind::UpDownCounter; }
};

template <class T>
class BoundNoopValueRecorder : public BoundValueRecorder<T>
{

public:
  BoundNoopValueRecorder() = default;

  BoundNoopValueRecorder(nostd::string_view /*name*/,
                         nostd::string_view /*description*/,
                         nostd::string_view /*unit*/,
                         bool /*enabled*/)
  {}

  virtual void record(T value) override {}

  virtual bool IsEnabled() override { return false; }

  virtual nostd::string_view GetName() override { return nostd::string_view(""); }

  virtual nostd::string_view GetDescription() override { return nostd::string_view(""); }

  virtual nostd::string_view GetUnits() override { return nostd::string_view(""); }

  virtual InstrumentKind GetKind() override { return InstrumentKind::ValueRecorder; }
};

template <class T>
class NoopValueRecorder : public ValueRecorder<T>
{

public:
  NoopValueRecorder() = default;

  NoopValueRecorder(nostd::string_view /*name*/,
                    nostd::string_view /*description*/,
                    nostd::string_view /*unit*/,
                    bool /*enabled*/)
  {}

  nostd::shared_ptr<BoundNoopValueRecorder<T>> bindNoopValueRecorder(
      const common::KeyValueIterable & /*labels*/)
  {
    return nostd::shared_ptr<BoundNoopValueRecorder<T>>(new BoundNoopValueRecorder<T>());
  }

  virtual void record(T value, const common::KeyValueIterable & /*labels*/) override {}

  virtual void update(T value, const common::KeyValueIterable & /*labels*/) override {}

  virtual bool IsEnabled() override { return false; }

  virtual nostd::string_view GetName() override { return nostd::string_view(""); }

  virtual nostd::string_view GetDescription() override { return nostd::string_view(""); }

  virtual nostd::string_view GetUnits() override { return nostd::string_view(""); }

  virtual InstrumentKind GetKind() override { return InstrumentKind::ValueRecorder; }
};

/**
 * No-op implementation of Meter. This class should not be used directly.
 */
class NoopMeter : public Meter
{
public:
  NoopMeter() = default;

  /**
   *
   * Creates a new NoopCounter<T> and returns a shared ptr to that counter.
   *
   * @param name the name of the instrument.
   * @param description a brief description of the instrument.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean that turns the metric instrument on and off.
   * @return
   */
  nostd::shared_ptr<Counter<short>> NewShortCounter(nostd::string_view name,
                                                    nostd::string_view description,
                                                    nostd::string_view unit,
                                                    const bool enabled) override
  {
    return nostd::shared_ptr<Counter<short>>{
        new NoopCounter<short>(name, description, unit, enabled)};
  }

  nostd::shared_ptr<Counter<int>> NewIntCounter(nostd::string_view name,
                                                nostd::string_view description,
                                                nostd::string_view unit,
                                                const bool enabled) override
  {
    return nostd::shared_ptr<Counter<int>>{new NoopCounter<int>(name, description, unit, enabled)};
  }

  nostd::shared_ptr<Counter<float>> NewFloatCounter(nostd::string_view name,
                                                    nostd::string_view description,
                                                    nostd::string_view unit,
                                                    const bool enabled) override
  {
    return nostd::shared_ptr<Counter<float>>{
        new NoopCounter<float>(name, description, unit, enabled)};
  }

  nostd::shared_ptr<Counter<double>> NewDoubleCounter(nostd::string_view name,
                                                      nostd::string_view description,
                                                      nostd::string_view unit,
                                                      const bool enabled) override
  {
    return nostd::shared_ptr<Counter<double>>{
        new NoopCounter<double>(name, description, unit, enabled)};
  }

  /**
   *
   * Creates a new NoopCounter<T> and returns a shared ptr to that counter.
   *
   * @param name the name of the instrument.
   * @param description a brief description of the instrument.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean that turns the metric instrument on and off.
   * @return
   */
  nostd::shared_ptr<UpDownCounter<short>> NewShortUpDownCounter(nostd::string_view name,
                                                                nostd::string_view description,
                                                                nostd::string_view unit,
                                                                const bool enabled) override
  {
    return nostd::shared_ptr<UpDownCounter<short>>{
        new NoopUpDownCounter<short>(name, description, unit, enabled)};
  }

  nostd::shared_ptr<UpDownCounter<int>> NewIntUpDownCounter(nostd::string_view name,
                                                            nostd::string_view description,
                                                            nostd::string_view unit,
                                                            const bool enabled) override
  {
    return nostd::shared_ptr<UpDownCounter<int>>{
        new NoopUpDownCounter<int>(name, description, unit, enabled)};
  }

  nostd::shared_ptr<UpDownCounter<float>> NewFloatUpDownCounter(nostd::string_view name,
                                                                nostd::string_view description,
                                                                nostd::string_view unit,
                                                                const bool enabled) override
  {
    return nostd::shared_ptr<UpDownCounter<float>>{
        new NoopUpDownCounter<float>(name, description, unit, enabled)};
  }

  nostd::shared_ptr<UpDownCounter<double>> NewDoubleUpDownCounter(nostd::string_view name,
                                                                  nostd::string_view description,
                                                                  nostd::string_view unit,
                                                                  const bool enabled) override
  {
    return nostd::shared_ptr<UpDownCounter<double>>{
        new NoopUpDownCounter<double>(name, description, unit, enabled)};
  }

  /**
   *
   * Creates a new ValueRecorder<T> and returns a shared ptr to that counter.
   *
   * @param name the name of the instrument.
   * @param description a brief description of the instrument.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean that turns the metric instrument on and off.
   * @return
   */
  nostd::shared_ptr<ValueRecorder<short>> NewShortValueRecorder(nostd::string_view name,
                                                                nostd::string_view description,
                                                                nostd::string_view unit,
                                                                const bool enabled) override
  {
    return nostd::shared_ptr<ValueRecorder<short>>{
        new NoopValueRecorder<short>(name, description, unit, enabled)};
  }

  nostd::shared_ptr<ValueRecorder<int>> NewIntValueRecorder(nostd::string_view name,
                                                            nostd::string_view description,
                                                            nostd::string_view unit,
                                                            const bool enabled) override
  {
    return nostd::shared_ptr<ValueRecorder<int>>{
        new NoopValueRecorder<int>(name, description, unit, enabled)};
  }

  nostd::shared_ptr<ValueRecorder<float>> NewFloatValueRecorder(nostd::string_view name,
                                                                nostd::string_view description,
                                                                nostd::string_view unit,
                                                                const bool enabled) override
  {
    return nostd::shared_ptr<ValueRecorder<float>>{
        new NoopValueRecorder<float>(name, description, unit, enabled)};
  }

  nostd::shared_ptr<ValueRecorder<double>> NewDoubleValueRecorder(nostd::string_view name,
                                                                  nostd::string_view description,
                                                                  nostd::string_view unit,
                                                                  const bool enabled) override
  {
    return nostd::shared_ptr<ValueRecorder<double>>{
        new NoopValueRecorder<double>(name, description, unit, enabled)};
  }

  /**
   *
   * Creates a new SumObserver<T> and returns a shared ptr to that counter.
   *
   * @param name the name of the instrument.
   * @param description a brief description of the instrument.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean that turns the metric instrument on and off.
   * @return
   */
  nostd::shared_ptr<SumObserver<short>> NewShortSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<short>)) override
  {
    return nostd::shared_ptr<SumObserver<short>>{
        new NoopSumObserver<short>(name, description, unit, enabled, callback)};
  }

  nostd::shared_ptr<SumObserver<int>> NewIntSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<int>)) override
  {
    return nostd::shared_ptr<SumObserver<int>>{
        new NoopSumObserver<int>(name, description, unit, enabled, callback)};
  }

  nostd::shared_ptr<SumObserver<float>> NewFloatSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<float>)) override
  {
    return nostd::shared_ptr<SumObserver<float>>{
        new NoopSumObserver<float>(name, description, unit, enabled, callback)};
  }

  nostd::shared_ptr<SumObserver<double>> NewDoubleSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<double>)) override
  {
    return nostd::shared_ptr<SumObserver<double>>{
        new NoopSumObserver<double>(name, description, unit, enabled, callback)};
  }

  /**
   *
   * Creates a new UpDownSumObserver<T> and returns a shared ptr to that counter.
   *
   * @param name the name of the instrument.
   * @param description a brief description of the instrument.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean that turns the metric instrument on and off.
   * @return
   */
  nostd::shared_ptr<UpDownSumObserver<short>> NewShortUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<short>)) override
  {
    return nostd::shared_ptr<UpDownSumObserver<short>>{
        new NoopUpDownSumObserver<short>(name, description, unit, enabled, callback)};
  }

  nostd::shared_ptr<UpDownSumObserver<int>> NewIntUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<int>)) override
  {
    return nostd::shared_ptr<UpDownSumObserver<int>>{
        new NoopUpDownSumObserver<int>(name, description, unit, enabled, callback)};
  }

  nostd::shared_ptr<UpDownSumObserver<float>> NewFloatUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<float>)) override
  {
    return nostd::shared_ptr<UpDownSumObserver<float>>{
        new NoopUpDownSumObserver<float>(name, description, unit, enabled, callback)};
  }

  nostd::shared_ptr<UpDownSumObserver<double>> NewDoubleUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<double>)) override
  {
    return nostd::shared_ptr<UpDownSumObserver<double>>{
        new NoopUpDownSumObserver<double>(name, description, unit, enabled, callback)};
  }

  /**
   *
   * Creates a new ValueObserverObserver<T> and returns a shared ptr to that counter.
   *
   * @param name the name of the instrument.
   * @param description a brief description of the instrument.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean that turns the metric instrument on and off.
   * @return
   */
  nostd::shared_ptr<ValueObserver<short>> NewShortValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<short>)) override
  {
    return nostd::shared_ptr<ValueObserver<short>>{
        new NoopValueObserver<short>(name, description, unit, enabled, callback)};
  }

  nostd::shared_ptr<ValueObserver<int>> NewIntValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<int>)) override
  {
    return nostd::shared_ptr<ValueObserver<int>>{
        new NoopValueObserver<int>(name, description, unit, enabled, callback)};
  }

  nostd::shared_ptr<ValueObserver<float>> NewFloatValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<float>)) override
  {
    return nostd::shared_ptr<ValueObserver<float>>{
        new NoopValueObserver<float>(name, description, unit, enabled, callback)};
  }

  nostd::shared_ptr<ValueObserver<double>> NewDoubleValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<double>)) override
  {
    return nostd::shared_ptr<ValueObserver<double>>{
        new NoopValueObserver<double>(name, description, unit, enabled, callback)};
  }

  /**
   *
   * Utility method that allows users to atomically record measurements to a set of
   * synchronous metric instruments with a common set of labels.
   *
   * @param labels the set of labels to associate with this recorder.
   * @param instrs the instruments to record to.
   * @param values the value to record to those instruments.
   */
  void RecordShortBatch(const common::KeyValueIterable &labels,
                        nostd::span<SynchronousInstrument<short> *> instruments,
                        nostd::span<const short> values) noexcept override
  {
    // No-op
  }

  void RecordIntBatch(const common::KeyValueIterable &labels,
                      nostd::span<SynchronousInstrument<int> *> instruments,
                      nostd::span<const int> values) noexcept override
  {
    // No-op
  }

  void RecordFloatBatch(const common::KeyValueIterable &labels,
                        nostd::span<SynchronousInstrument<float> *> instruments,
                        nostd::span<const float> values) noexcept override
  {
    // No-op
  }

  void RecordDoubleBatch(const common::KeyValueIterable &labels,
                         nostd::span<SynchronousInstrument<double> *> instruments,
                         nostd::span<const double> values) noexcept override
  {
    // No-op
  }
};

class NoopMeterProvider final : public opentelemetry::metrics::MeterProvider
{
public:
  NoopMeterProvider()
      : meter_{nostd::shared_ptr<opentelemetry::metrics::NoopMeter>(
            new opentelemetry::metrics::NoopMeter)}
  {}

  nostd::shared_ptr<opentelemetry::metrics::Meter> GetMeter(
      nostd::string_view library_name,
      nostd::string_view library_version) override
  {
    return meter_;
  }

private:
  nostd::shared_ptr<opentelemetry::metrics::Meter> meter_;
};
}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
#endif
