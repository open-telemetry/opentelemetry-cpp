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


class BoundNoopIntCounter : public BoundIntCounter {

public:

    BoundNoopIntCounter() = default;

    BoundNoopIntCounter(nostd::string_view /*name*/, nostd::string_view /*description*/, nostd::string_view /*unit*/, bool /*enabled*/){}

    void add(int value){}

    void unbind();
};

class NoopIntCounter : public IntCounter {

public:

    NoopIntCounter() =  default;

    NoopIntCounter(nostd::string_view /*name*/, nostd::string_view /*description*/, nostd::string_view /*unit*/, bool /*enabled*/) {}
    

    BoundNoopIntCounter bind(const nostd::string_view & /*labels*/){
        return BoundNoopIntCounter();
    }

    void add(int value, const std::string_view & /*labels*/){}

};


class BoundNoopDoubleCounter : public DoubleCounter {

public:

    BoundNoopDoubleCounter() = default;

    BoundNoopDoubleCounter(nostd::string_view /*name*/, nostd::string_view /*description*/, nostd::string_view /*unit*/, bool /*enabled*/){}

    void add(double value){}

    void unbind();
};

class NoopDoubleCounter : public DoubleCounter {

public:

    NoopDoubleCounter()=default;

    NoopDoubleCounter(nostd::string_view /*name*/, nostd::string_view /*description*/, nostd::string_view /*unit*/, bool /*enabled*/) {}
    

    BoundNoopDoubleCounter bind(const nostd::string_view & /*labels*/){
        return BoundNoopDoubleCounter();
    }

    void add(double value, const std::string_view & /*labels*/){}

};

class BoundNoopIntUpDownCounter : public BoundIntUpDownCounter {

public:

    BoundNoopIntUpDownCounter() = default;

    BoundNoopIntUpDownCounter(nostd::string_view /*name*/, nostd::string_view /*description*/, nostd::string_view /*unit*/, bool /*enabled*/){}

    void add(int value){}

    void unbind();
};

class NoopIntUpDownCounter : public IntUpDownCounter {

public:

    NoopIntUpDownCounter()=default;

    NoopIntUpDownCounter(nostd::string_view /*name*/, nostd::string_view /*description*/, nostd::string_view /*unit*/, bool /*enabled*/) {}
    

    BoundNoopIntUpDownCounter bind(const nostd::string_view & /*labels*/){
        return BoundNoopIntUpDownCounter();
    }

    void add(int value, const std::string_view & /*labels*/){}
};

class BoundNoopDoubleUpDownCounter : public DoubleUpDownCounter {

public:

    BoundNoopDoubleUpDownCounter() = default;

    BoundNoopDoubleUpDownCounter(nostd::string_view /*name*/, nostd::string_view /*description*/, nostd::string_view /*unit*/, bool /*enabled*/){}

    void add(double value){}

    void unbind();
};

class NoopDoubleUpDownCounter : public DoubleUpDownCounter {

public:

    NoopDoubleUpDownCounter()=default;

    NoopDoubleUpDownCounter(nostd::string_view /*name*/, nostd::string_view /*description*/, nostd::string_view /*unit*/, bool /*enabled*/) {}
    

    BoundNoopIntUpDownCounter bind(const nostd::string_view & /*labels*/){
        return BoundNoopIntUpDownCounter();
    }

    void add(double value, const std::string_view & /*labels*/){}
};

class BoundNoopIntValueRecorder : public IntValueRecorder {

public:

    BoundNoopIntValueRecorder() = default;

    BoundNoopIntValueRecorder(nostd::string_view /*name*/, nostd::string_view /*description*/, nostd::string_view /*unit*/, bool /*enabled*/){}

    void record(int value){}

    void unbind();
};

class NoopIntValueRecorder : public IntValueRecorder {

public:

    NoopIntValueRecorder()=default;

    NoopIntValueRecorder(nostd::string_view /*name*/, nostd::string_view /*description*/, nostd::string_view /*unit*/, bool /*enabled*/) {}
    

    BoundNoopIntValueRecorder bind(const nostd::string_view & /*labels*/){
        return BoundNoopIntValueRecorder();
    }

    void record(int value, const std::string_view & /*labels*/){}
};

class BoundNoopDoubleValueRecorder : public DoubleValueRecorder {

public:

    BoundNoopDoubleValueRecorder() = default;

    BoundNoopDoubleValueRecorder(nostd::string_view /*name*/, nostd::string_view /*description*/, nostd::string_view /*unit*/, bool /*enabled*/){}

    void record(int value){}

    void unbind();
};

class NoopDoubleValueRecorder : public DoubleValueRecorder {

public:

    NoopDoubleValueRecorder()=default;

    NoopDoubleValueRecorder(nostd::string_view /*name*/, nostd::string_view /*description*/, nostd::string_view /*unit*/, bool /*enabled*/) {}
    

    BoundNoopDoubleValueRecorder bind(const nostd::string_view & /*labels*/){
        return BoundNoopDoubleValueRecorder();
    }

    void record(double value, const std::string_view & /*labels*/){}
};

} // namespace metrics 
OPENTELEMETRY_END_NAMESPACE
