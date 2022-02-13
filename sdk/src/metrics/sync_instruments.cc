// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/sync_instruments.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
LongCounter::LongCounter(
    nostd::string_view name,
    const sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
    MeasurementProcessor *measurement_processor,
    nostd::string_view description,
    nostd::string_view unit)
    : Synchronous(name, instrumentation_library, measurement_processor, description, unit)
{}

void LongCounter::Add(long value,
                      const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  return measurement_processor_->RecordLong(value, attributes);
}

void LongCounter::Add(long value) noexcept
{
  return measurement_processor_->RecordLong(value);
}

DoubleCounter::DoubleCounter(
    nostd::string_view name,
    const sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
    MeasurementProcessor *measurement_processor,
    nostd::string_view description,
    nostd::string_view unit)
    : Synchronous(name, instrumentation_library, measurement_processor, description, unit)
{}

void DoubleCounter::Add(double value,
                        const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  return measurement_processor_->RecordDouble(value, attributes);
}

void DoubleCounter::Add(double value) noexcept
{
  return measurement_processor_->RecordDouble(value);
}

LongUpDownCounter::LongUpDownCounter(
    nostd::string_view name,
    const sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
    MeasurementProcessor *measurement_processor,
    nostd::string_view description,
    nostd::string_view unit)
    : Synchronous(name, instrumentation_library, measurement_processor, description, unit)
{}

void LongUpDownCounter::Add(long value,
                            const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  return measurement_processor_->RecordLong(value, attributes);
}

void LongUpDownCounter::Add(long value) noexcept
{
  return measurement_processor_->RecordLong(value);
}

DoubleUpDownCounter::DoubleUpDownCounter(
    nostd::string_view name,
    const sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
    MeasurementProcessor *measurement_processor,
    nostd::string_view description,
    nostd::string_view unit)
    : Synchronous(name, instrumentation_library, measurement_processor, description, unit)
{}

void DoubleUpDownCounter::Add(double value,
                              const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  return measurement_processor_->RecordDouble(value, attributes);
}

void DoubleUpDownCounter::Add(double value) noexcept
{
  return measurement_processor_->RecordDouble(value);
}

LongHistogram::LongHistogram(
    nostd::string_view name,
    const sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
    MeasurementProcessor *measurement_processor,
    nostd::string_view description,
    nostd::string_view unit)
    : Synchronous(name, instrumentation_library, measurement_processor, description, unit)
{}

void LongHistogram::Record(long value,
                           const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  return measurement_processor_->RecordLong(value, attributes);
}

void LongHistogram::Record(long value) noexcept
{
  return measurement_processor_->RecordLong(value);
}

DoubleHistogram::DoubleHistogram(
    nostd::string_view name,
    const sdk::instrumentationlibrary::InstrumentationLibrary *instrumentation_library,
    MeasurementProcessor *measurement_processor,
    nostd::string_view description,
    nostd::string_view unit)
    : Synchronous(name, instrumentation_library, measurement_processor, description, unit)
{}

void DoubleHistogram::Record(double value,
                             const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  return measurement_processor_->RecordDouble(value, attributes);
}

void DoubleHistogram::Record(double value) noexcept
{
  return measurement_processor_->RecordDouble(value);
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
