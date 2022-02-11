// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/key_value_iterable.h"
#  include "opentelemetry/metrics/sync_instruments.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/sdk/metrics/measurement_processor.h"

#  include "opentelemetry/sdk/instrumentationlibrary/instrumentation_library.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class Synchronous
{
public:
  Synchronous(nostd::string_view name,
              const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
                  *instrumentation_library,
              MeasurementProcessor *measurement_processor,
              nostd::string_view description = "",
              nostd::string_view unit        = "")
      : name_(name),
        instrumentation_library_(instrumentation_library),
        measurement_processor_(measurement_processor),
        description_(description),
        unit_(unit)
  {}

protected:
  std::string name_;
  const sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library_;
  MeasurementProcessor *measurement_processor_;
  std::string description_;
  std::string unit_;
};

class LongCounter : public Synchronous, public opentelemetry::metrics::Counter<long>
{
public:
  LongCounter(nostd::string_view name,
              const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
                  *instrumentation_library,
              MeasurementProcessor *measurement_processor,
              nostd::string_view description = "",
              nostd::string_view unit        = "");

  void Add(long value, const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void Add(long value) noexcept override;
};

class DoubleCounter : public Synchronous, public opentelemetry::metrics::Counter<double>
{

public:
  DoubleCounter(nostd::string_view name,
                const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
                    *instrumentation_library,
                MeasurementProcessor *measurement_processor,
                nostd::string_view description = "",
                nostd::string_view unit        = "");

  void Add(double value,
           const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void Add(double value) noexcept override;
};

class LongUpDownCounter : public Synchronous, public opentelemetry::metrics::UpDownCounter<long>
{
public:
  LongUpDownCounter(nostd::string_view name,
                    const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
                        *instrumentation_library,
                    MeasurementProcessor *measurement_processor,
                    nostd::string_view description = "",
                    nostd::string_view unit        = "");

  void Add(long value, const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void Add(long value) noexcept override;
};

class DoubleUpDownCounter : public Synchronous, public opentelemetry::metrics::UpDownCounter<double>
{
public:
  DoubleUpDownCounter(nostd::string_view name,
                      const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
                          *instrumentation_library,
                      MeasurementProcessor *measurement_processor,
                      nostd::string_view description = "",
                      nostd::string_view unit        = "");

  void Add(double value,
           const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void Add(double value) noexcept override;
};

class LongHistogram : public Synchronous, public opentelemetry::metrics::Histogram<long>
{
public:
  LongHistogram(nostd::string_view name,
                const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
                    *instrumentation_library,
                MeasurementProcessor *measurement_processor,
                nostd::string_view description = "",
                nostd::string_view unit        = "");

  void Record(long value,
              const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void Record(long value) noexcept override;
};

class DoubleHistogram : public Synchronous, public opentelemetry::metrics::Histogram<double>
{
public:
  DoubleHistogram(nostd::string_view name,
                  const opentelemetry::sdk::instrumentationlibrary::InstrumentationLibrary
                      *instrumentation_library,
                  MeasurementProcessor *measurement_processor,
                  nostd::string_view description = "",
                  nostd::string_view unit        = "");

  void Record(double value,
              const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void Record(double value) noexcept override;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif