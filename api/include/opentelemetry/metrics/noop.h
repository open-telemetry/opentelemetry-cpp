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

class NoopIntValueObserver : public IntValueObserver
{

public:
  NoopIntValueObserver(nostd::string_view /*name*/,
                       nostd::string_view /*description*/,
                       nostd::string_view /*unit*/,
                       bool /*enabled*/,
                       void (*callback)(ObserverResult))
  {}
};

class NoopDoubleValueObserver : public DoubleValueObserver
{

public:
  NoopDoubleValueObserver(nostd::string_view /*name*/,
                          nostd::string_view /*description*/,
                          nostd::string_view /*unit*/,
                          bool enabled,
                          void (*callback)(ObserverResult))
  {}
};

class NoopIntSumObserver : public IntSumObserver
{

public:
  NoopIntSumObserver(nostd::string_view /*name*/,
                     nostd::string_view /*description*/,
                     nostd::string_view /*unit*/,
                     bool /*enabled*/,
                     void (*callback)(ObserverResult))
  {}
};

class NoopDoubleSumObserver : public DoubleSumObserver
{

public:
  NoopDoubleSumObserver(nostd::string_view /*name*/,
                        nostd::string_view /*description*/,
                        nostd::string_view /*unit*/,
                        bool /*enabled*/,
                        void (*callback)(ObserverResult))
  {}
};

class NoopIntUpDownSumObserver : public IntUpDownSumObserver
{

public:
  NoopIntUpDownSumObserver(nostd::string_view /*name*/,
                           nostd::string_view /*description*/,
                           nostd::string_view /*unit*/,
                           bool /*enabled*/,
                           void (*callback)(ObserverResult))
  {}
};

class NoopDoubleUpDownSumObserver : public DoubleUpDownSumObserver
{

public:
  NoopDoubleUpDownSumObserver(nostd::string_view /*name*/,
                              nostd::string_view /*description*/,
                              nostd::string_view /*unit*/,
                              bool /*enabled*/,
                              void (*callback)(ObserverResult))
  {}
};

class BoundNoopIntCounter : public BoundIntCounter
{

public:
  BoundNoopIntCounter() = default;

  BoundNoopIntCounter(nostd::string_view /*name*/,
                      nostd::string_view /*description*/,
                      nostd::string_view /*unit*/,
                      bool /*enabled*/)
  {}

  virtual void add(int value) override {}
};

class NoopIntCounter : public IntCounter
{

public:
  NoopIntCounter() = default;

  NoopIntCounter(nostd::string_view /*name*/,
                 nostd::string_view /*description*/,
                 nostd::string_view /*unit*/,
                 bool /*enabled*/)
  {}

  nostd::shared_ptr<BoundNoopIntCounter> bind(const nostd::string_view & /*labels*/)
  {
    return nostd::shared_ptr<BoundNoopIntCounter>(new BoundNoopIntCounter());
  }

  virtual void add(int value, const nostd::string_view & /*labels*/) override {}
};

class BoundNoopDoubleCounter : public BoundDoubleCounter
{

public:
  BoundNoopDoubleCounter() = default;

  BoundNoopDoubleCounter(nostd::string_view /*name*/,
                         nostd::string_view /*description*/,
                         nostd::string_view /*unit*/,
                         bool /*enabled*/)
  {}

  virtual void add(double value) override {}
};

class NoopDoubleCounter : public DoubleCounter
{

public:
  NoopDoubleCounter() = default;

  NoopDoubleCounter(nostd::string_view /*name*/,
                    nostd::string_view /*description*/,
                    nostd::string_view /*unit*/,
                    bool /*enabled*/)
  {}

  nostd::shared_ptr<BoundNoopDoubleCounter> bind(const nostd::string_view & /*labels*/)
  {
    return nostd::shared_ptr<BoundNoopDoubleCounter>(new BoundNoopDoubleCounter());
  }

  virtual void add(double value, const nostd::string_view & /*labels*/) override {}
};

class BoundNoopIntUpDownCounter : public BoundIntUpDownCounter
{

public:
  BoundNoopIntUpDownCounter() = default;

  BoundNoopIntUpDownCounter(nostd::string_view /*name*/,
                            nostd::string_view /*description*/,
                            nostd::string_view /*unit*/,
                            bool /*enabled*/)
  {}

  virtual void add(int value) override {}
};

class NoopIntUpDownCounter : public IntUpDownCounter
{

public:
  NoopIntUpDownCounter() = default;

  NoopIntUpDownCounter(nostd::string_view /*name*/,
                       nostd::string_view /*description*/,
                       nostd::string_view /*unit*/,
                       bool /*enabled*/)
  {}

  nostd::shared_ptr<BoundNoopIntUpDownCounter> bind(const nostd::string_view & /*labels*/)
  {
    return nostd::shared_ptr<BoundNoopIntUpDownCounter>(new BoundNoopIntUpDownCounter());
  }

  virtual void add(int value, const nostd::string_view & /*labels*/) override {}
};

class BoundNoopDoubleUpDownCounter : public BoundDoubleUpDownCounter
{

public:
  BoundNoopDoubleUpDownCounter() = default;

  BoundNoopDoubleUpDownCounter(nostd::string_view /*name*/,
                               nostd::string_view /*description*/,
                               nostd::string_view /*unit*/,
                               bool /*enabled*/)
  {}

  virtual void add(double value) override {}
};

class NoopDoubleUpDownCounter : public DoubleUpDownCounter
{

public:
  NoopDoubleUpDownCounter() = default;

  NoopDoubleUpDownCounter(nostd::string_view /*name*/,
                          nostd::string_view /*description*/,
                          nostd::string_view /*unit*/,
                          bool /*enabled*/)
  {}

  nostd::shared_ptr<BoundNoopDoubleUpDownCounter> bind(const nostd::string_view & /*labels*/)
  {
    return nostd::shared_ptr<BoundNoopDoubleUpDownCounter>(new BoundNoopDoubleUpDownCounter());
  }

  virtual void add(double value, const nostd::string_view & /*labels*/) override {}
};

class BoundNoopIntValueRecorder : public BoundIntValueRecorder
{

public:
  BoundNoopIntValueRecorder() = default;

  BoundNoopIntValueRecorder(nostd::string_view /*name*/,
                            nostd::string_view /*description*/,
                            nostd::string_view /*unit*/,
                            bool /*enabled*/)
  {}

  virtual void record(int value) override {}
};

class NoopIntValueRecorder : public IntValueRecorder
{

public:
  NoopIntValueRecorder() = default;

  NoopIntValueRecorder(nostd::string_view /*name*/,
                       nostd::string_view /*description*/,
                       nostd::string_view /*unit*/,
                       bool /*enabled*/)
  {}

  nostd::shared_ptr<BoundNoopIntValueRecorder> bind(const nostd::string_view & /*labels*/)
  {
    return nostd::shared_ptr<BoundNoopIntValueRecorder>(new BoundNoopIntValueRecorder());
  }

  virtual void record(int value, const nostd::string_view & /*labels*/) override {}
};

class BoundNoopDoubleValueRecorder : public BoundDoubleValueRecorder
{

public:
  BoundNoopDoubleValueRecorder() = default;

  BoundNoopDoubleValueRecorder(nostd::string_view /*name*/,
                               nostd::string_view /*description*/,
                               nostd::string_view /*unit*/,
                               bool /*enabled*/)
  {}

  virtual void record(double value) override {}
};

class NoopDoubleValueRecorder : public DoubleValueRecorder
{

public:
  NoopDoubleValueRecorder() = default;

  NoopDoubleValueRecorder(nostd::string_view /*name*/,
                          nostd::string_view /*description*/,
                          nostd::string_view /*unit*/,
                          bool /*enabled*/)
  {}

  nostd::shared_ptr<BoundNoopDoubleValueRecorder> bind(const nostd::string_view & /*labels*/)
  {
    return nostd::shared_ptr<BoundNoopDoubleValueRecorder>(new BoundNoopDoubleValueRecorder());
  }

  virtual void record(double value, const nostd::string_view & /*labels*/) override {}
};

} // namespace metrics 
OPENTELEMETRY_END_NAMESPACE
