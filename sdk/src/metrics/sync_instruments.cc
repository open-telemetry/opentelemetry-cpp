// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/sync_instruments.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
LongCounter::LongCounter(InstrumentDescriptor instrument_descriptor,
                         std::unique_ptr<WritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{}

void LongCounter::Add(long value,
                      const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  return storage_->RecordLong(value, attributes);
}

void LongCounter::Add(long value) noexcept
{
  return storage_->RecordLong(value);
}

DoubleCounter::DoubleCounter(InstrumentDescriptor instrument_descriptor,
                             std::unique_ptr<WritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{}

void DoubleCounter::Add(double value,
                        const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  return storage_->RecordDouble(value, attributes);
}

void DoubleCounter::Add(double value) noexcept
{
  return storage_->RecordDouble(value);
}

LongUpDownCounter::LongUpDownCounter(InstrumentDescriptor instrument_descriptor,
                                     std::unique_ptr<WritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{}

void LongUpDownCounter::Add(long value,
                            const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  return storage_->RecordLong(value, attributes);
}

void LongUpDownCounter::Add(long value) noexcept
{
  return storage_->RecordLong(value);
}

DoubleUpDownCounter::DoubleUpDownCounter(InstrumentDescriptor instrument_descriptor,
                                         std::unique_ptr<WritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{}

void DoubleUpDownCounter::Add(double value,
                              const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  return storage_->RecordDouble(value, attributes);
}

void DoubleUpDownCounter::Add(double value) noexcept
{
  return storage_->RecordDouble(value);
}

LongHistogram::LongHistogram(InstrumentDescriptor instrument_descriptor,
                             std::unique_ptr<WritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{}

void LongHistogram::Record(long value,
                           const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  return storage_->RecordLong(value, attributes);
}

void LongHistogram::Record(long value) noexcept
{
  return storage_->RecordLong(value);
}

DoubleHistogram::DoubleHistogram(InstrumentDescriptor instrument_descriptor,
                                 std::unique_ptr<WritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{}

void DoubleHistogram::Record(double value,
                             const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  return storage_->RecordDouble(value, attributes);
}

void DoubleHistogram::Record(double value) noexcept
{
  return storage_->RecordDouble(value);
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
