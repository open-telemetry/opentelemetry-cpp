// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/sync_instruments.h"

#include <cmath>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
DoubleCounter::DoubleCounter(InstrumentDescriptor instrument_descriptor,
                             std::unique_ptr<SyncWritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[DoubleCounter::DoubleCounter] - Error during constructing DoubleCounter."
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

void LongUpDownCounter::Add(int64_t value,
                            const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  auto context = opentelemetry::context::Context{};
  if (!storage_)
  {
    return;
  }
  return storage_->RecordLong(value, attributes, context);
}

void LongUpDownCounter::Add(int64_t value,
                            const opentelemetry::common::KeyValueIterable &attributes,
                            const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    return;
  }
  return storage_->RecordLong(value, attributes, context);
}

void LongUpDownCounter::Add(int64_t value) noexcept
{
  auto context = opentelemetry::context::Context{};
  if (!storage_)
  {
    return;
  }
  return storage_->RecordLong(value, context);
}

void LongUpDownCounter::Add(int64_t value, const opentelemetry::context::Context &context) noexcept
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
        "[DoubleUpDownCounter::DoubleUpDownCounter] - Error during constructing "
        "DoubleUpDownCounter."
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

DoubleHistogram::DoubleHistogram(InstrumentDescriptor instrument_descriptor,
                                 std::unique_ptr<SyncWritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[DoubleHistogram::DoubleHistogram] - Error during constructing DoubleHistogram."
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
