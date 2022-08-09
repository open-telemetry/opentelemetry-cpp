// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/sync_instruments.h"
#  include "opentelemetry/sdk/metrics/state/metric_storage.h"
#  include "opentelemetry/sdk_config.h"

#  include <cmath>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
LongCounter::LongCounter(InstrumentDescriptor instrument_descriptor,
                         std::unique_ptr<SyncWritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_ERROR("[LongCounter::LongCounter] - Error during constructing LongCounter."
                            << "The metric storage is invalid"
                            << "No value will be added");
  }
}

void LongCounter::Add(long value,
                      const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  if (!storage_)
  {
    return;
  }
  auto context = opentelemetry::context::Context{};
  return storage_->RecordLong(value, attributes, context);
}

void LongCounter::Add(long value,
                      const opentelemetry::common::KeyValueIterable &attributes,
                      const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    return;
  }
  return storage_->RecordLong(value, attributes, context);
}

void LongCounter::Add(long value) noexcept
{
  auto context = opentelemetry::context::Context{};
  if (!storage_)
  {
    return;
  }
  return storage_->RecordLong(value, context);
}

void LongCounter::Add(long value, const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    return;
  }
  return storage_->RecordLong(value, context);
}

DoubleCounter::DoubleCounter(InstrumentDescriptor instrument_descriptor,
                             std::unique_ptr<SyncWritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[LongUpDownCounter::LongUpDownCounter] - Error during constructing LongUpDownCounter."
        << "The metric storage is invalid"
        << "No value will be added");
  }
}

void DoubleCounter::Add(double value,
                        const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  auto context = opentelemetry::context::Context{};
  if (!storage_)
  {
    return;
  }
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleCounter::Add(double value,
                        const opentelemetry::common::KeyValueIterable &attributes,
                        const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    return;
  }
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleCounter::Add(double value) noexcept
{
  auto context = opentelemetry::context::Context{};
  if (!storage_)
  {
    return;
  }
  return storage_->RecordDouble(value, context);
}

void DoubleCounter::Add(double value, const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    return;
  }
  return storage_->RecordDouble(value, context);
}

LongUpDownCounter::LongUpDownCounter(InstrumentDescriptor instrument_descriptor,
                                     std::unique_ptr<SyncWritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[LongUpDownCounter::LongUpDownCounter] - Error during constructing LongUpDownCounter."
        << "The metric storage is invalid"
        << "No value will be added");
  }
}

void LongUpDownCounter::Add(long value,
                            const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  auto context = opentelemetry::context::Context{};
  if (!storage_)
  {
    return;
  }
  return storage_->RecordLong(value, attributes, context);
}

void LongUpDownCounter::Add(long value,
                            const opentelemetry::common::KeyValueIterable &attributes,
                            const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    return;
  }
  return storage_->RecordLong(value, attributes, context);
}

void LongUpDownCounter::Add(long value) noexcept
{
  auto context = opentelemetry::context::Context{};
  if (!storage_)
  {
    return;
  }
  return storage_->RecordLong(value, context);
}

void LongUpDownCounter::Add(long value, const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    return;
  }
  return storage_->RecordLong(value, context);
}

DoubleUpDownCounter::DoubleUpDownCounter(InstrumentDescriptor instrument_descriptor,
                                         std::unique_ptr<SyncWritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[LongUpDownCounter::LongUpDownCounter] - Error during constructing LongUpDownCounter."
        << "The metric storage is invalid"
        << "No value will be added");
  }
}

void DoubleUpDownCounter::Add(double value,
                              const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  auto context = opentelemetry::context::Context{};
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleUpDownCounter::Add(double value,
                              const opentelemetry::common::KeyValueIterable &attributes,
                              const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    return;
  }
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleUpDownCounter::Add(double value) noexcept
{
  if (!storage_)
  {
    return;
  }
  auto context = opentelemetry::context::Context{};
  return storage_->RecordDouble(value, context);
}

void DoubleUpDownCounter::Add(double value, const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    return;
  }
  return storage_->RecordDouble(value, context);
}

LongHistogram::LongHistogram(InstrumentDescriptor instrument_descriptor,
                             std::unique_ptr<SyncWritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[LongUpDownCounter::LongUpDownCounter] - Error during constructing LongUpDownCounter."
        << "The metric storage is invalid"
        << "No value will be added");
  }
}

void LongHistogram::Record(long value,
                           const opentelemetry::common::KeyValueIterable &attributes,
                           const opentelemetry::context::Context &context) noexcept
{
  if (value < 0)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[LongHistogram::Record(value, attributes)] negative value provided to histogram Name:"
        << instrument_descriptor_.name_ << " Value:" << value);
    return;
  }
  return storage_->RecordLong(value, attributes, context);
}

void LongHistogram::Record(long value, const opentelemetry::context::Context &context) noexcept
{
  if (value < 0)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[LongHistogram::Record(value)] negative value provided to histogram Name:"
        << instrument_descriptor_.name_ << " Value:" << value);
    return;
  }
  return storage_->RecordLong(value, context);
}

DoubleHistogram::DoubleHistogram(InstrumentDescriptor instrument_descriptor,
                                 std::unique_ptr<SyncWritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[LongUpDownCounter::LongUpDownCounter] - Error during constructing LongUpDownCounter."
        << "The metric storage is invalid"
        << "No value will be added");
  }
}

void DoubleHistogram::Record(double value,
                             const opentelemetry::common::KeyValueIterable &attributes,
                             const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    return;
  }
  if (value < 0 || std::isnan(value) || std::isinf(value))
  {
    OTEL_INTERNAL_LOG_WARN(
        "[DoubleHistogram::Record(value, attributes)] negative/nan/infinite value provided to "
        "histogram Name:"
        << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleHistogram::Record(double value, const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    return;
  }
  if (value < 0 || std::isnan(value) || std::isinf(value))
  {
    OTEL_INTERNAL_LOG_WARN(
        "[DoubleHistogram::Record(value)] negative/nan/infinite value provided to histogram Name:"
        << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordDouble(value, context);
}

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
