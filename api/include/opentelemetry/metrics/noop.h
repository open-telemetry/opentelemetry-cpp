#pragma once
// Please refer to provider.h for documentation on how to obtain a Meter object.
//
// This file is part of the internal implementation of OpenTelemetry. Nothing in this file should be
// used directly. Please refer to meter.h for documentation on these interfaces.

#include "opentelemetry/version.h"
#include "opentelemetry/metrics/instrument.h"
#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/metrics/meter_provider.h"
#include "opentelemetry/metrics/sync_instruments.h"
#include "opentelemetry/metrics/async_instruments.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"

#include <memory>


OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{
/**
 * No-op implementation of a MeterProvider.
 */
class NoopMeter final : public Meter, public std::enable_shared_from_this<NoopMeter> {};

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

template <class T>
class NoopValueObserver : public ValueObserver<T>
{
    
public:
    NoopValueObserver(nostd::string_view /*name*/,
                      nostd::string_view /*description*/,
                      nostd::string_view /*unit*/,
                      bool /*enabled*/,
                      void (*callback)(ObserverResult<T>)) {}
    
    virtual bool IsEnabled() override {
        return false;
    }
    
    virtual nostd::string_view GetName() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetDescription() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetUnits() override {
        return nostd::string_view("");
    }
    
    virtual void observe(T value, const trace::KeyValueIterable &labels) override {}
    
    virtual void run() override {}
    
    virtual InstrumentKind GetKind() override {
        return InstrumentKind::Counter;
    }
    
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
    
    virtual bool IsEnabled() override {
        return false;
    }
    
    virtual nostd::string_view GetName() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetDescription() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetUnits() override {
        return nostd::string_view("");
    }
    
    virtual void observe(T value, const trace::KeyValueIterable &labels) override {}
    
    virtual void run() override {}
    
    virtual InstrumentKind GetKind() override {
        return InstrumentKind::Counter;
    }
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
    
    virtual bool IsEnabled() override {
        return false;
    }
    
    virtual nostd::string_view GetName() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetDescription() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetUnits() override {
        return nostd::string_view("");
    }
    
    virtual void observe(T value, const trace::KeyValueIterable &labels) override {}
    
    virtual void run() override {}
    
    virtual InstrumentKind GetKind() override {
        return InstrumentKind::Counter;
    }
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
    
    virtual bool IsEnabled() override {
        return false;
    }
    
    virtual nostd::string_view GetName() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetDescription() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetUnits() override {
        return nostd::string_view("");
    }
    
    virtual InstrumentKind GetKind() override {
        return InstrumentKind::Counter;
    }
    
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
    
    nostd::shared_ptr<BoundNoopCounter<T>> bindNoopCounter(const trace::KeyValueIterable & /*labels*/)
    {
        return nostd::shared_ptr<BoundNoopCounter<T>>(new BoundNoopCounter<T>());
    }
    
    virtual void add(T value, const trace::KeyValueIterable & /*labels*/) override {}
    
    virtual void update(T value, const trace::KeyValueIterable & /*labels*/) override  {}
    
    virtual bool IsEnabled() override {
        return false;
    }
    
    virtual nostd::string_view GetName() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetDescription() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetUnits() override {
        return nostd::string_view("");
    }
    
    virtual InstrumentKind GetKind() override {
        return InstrumentKind::Counter;
    }
    
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
    
    virtual bool IsEnabled() override {
        return false;
    }
    
    virtual nostd::string_view GetName() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetDescription() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetUnits() override {
        return nostd::string_view("");
    }
    
    virtual InstrumentKind GetKind() override {
        return InstrumentKind::UpDownCounter;
    }
    
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
    
    nostd::shared_ptr<BoundNoopUpDownCounter<T>> bindNoopUpDownCounter(const trace::KeyValueIterable & /*labels*/)
    {
        return nostd::shared_ptr<BoundNoopUpDownCounter<T>>(new BoundNoopUpDownCounter<T>());
    }
    
    virtual void add(T value, const trace::KeyValueIterable & /*labels*/) override {}
    
    virtual void update(T value, const trace::KeyValueIterable & /*labels*/) override {}
    
    virtual bool IsEnabled() override {
        return false;
    }
    
    virtual nostd::string_view GetName() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetDescription() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetUnits() override {
        return nostd::string_view("");
    }
    
    virtual InstrumentKind GetKind() override {
        return InstrumentKind::UpDownCounter;
    }
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
    
    virtual bool IsEnabled() override {
        return false;
    }
    
    virtual nostd::string_view GetName() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetDescription() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetUnits() override {
        return nostd::string_view("");
    }
    
    virtual InstrumentKind GetKind() override {
        return InstrumentKind::ValueRecorder;
    }
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
    
    nostd::shared_ptr<BoundNoopValueRecorder<T>> bindNoopValueRecorder(const trace::KeyValueIterable & /*labels*/)
    {
        return nostd::shared_ptr<BoundNoopValueRecorder<T>>(new BoundNoopValueRecorder<T>());
    }
    
    virtual void record(T value, const trace::KeyValueIterable & /*labels*/) override {}
    
    virtual void update(T value, const trace::KeyValueIterable & /*labels*/) override {}
    
    virtual bool IsEnabled() override {
        return false;
    }
    
    virtual nostd::string_view GetName() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetDescription() override {
        return nostd::string_view("");
    }
    
    virtual nostd::string_view GetUnits() override {
        return nostd::string_view("");
    }
    
    virtual InstrumentKind GetKind() override {
        return InstrumentKind::ValueRecorder;
    }
    
};


} // namespace metrics 
OPENTELEMETRY_END_NAMESPACE
