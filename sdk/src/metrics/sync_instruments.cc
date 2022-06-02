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
                         std::unique_ptr<WritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{}

void LongCounter::Add(long value,
                      const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  auto context = opentelemetry::context::Context{};
  return storage_->RecordLong(value, attributes, context);
}

void LongCounter::Add(long value,
                      const opentelemetry::common::KeyValueIterable &attributes,
                      const opentelemetry::context::Context &context) noexcept
{
  return storage_->RecordLong(value, attributes, context);
}

void LongCounter::Add(long value) noexcept
{
  auto context = opentelemetry::context::Context{};
  return storage_->RecordLong(value, context);
}

void LongCounter::Add(long value, const opentelemetry::context::Context &context) noexcept
{
  return storage_->RecordLong(value, context);
}

DoubleCounter::DoubleCounter(InstrumentDescriptor instrument_descriptor,
                             std::unique_ptr<WritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{}

void DoubleCounter::Add(double value,
                        const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  auto context = opentelemetry::context::Context{};
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleCounter::Add(double value,
                        const opentelemetry::common::KeyValueIterable &attributes,
                        const opentelemetry::context::Context &context) noexcept
{
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleCounter::Add(double value) noexcept
{
  auto context = opentelemetry::context::Context{};
  return storage_->RecordDouble(value, context);
}

void DoubleCounter::Add(double value, const opentelemetry::context::Context &context) noexcept
{
  return storage_->RecordDouble(value, context);
}

LongUpDownCounter::LongUpDownCounter(InstrumentDescriptor instrument_descriptor,
                                     std::unique_ptr<WritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{}

void LongUpDownCounter::Add(long value,
                            const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  auto context = opentelemetry::context::Context{};
  return storage_->RecordLong(value, attributes, context);
}

void LongUpDownCounter::Add(long value,
                            const opentelemetry::common::KeyValueIterable &attributes,
                            const opentelemetry::context::Context &context) noexcept
{
  return storage_->RecordLong(value, attributes, context);
}

void LongUpDownCounter::Add(long value) noexcept
{
  auto context = opentelemetry::context::Context{};
  return storage_->RecordLong(value, context);
}

void LongUpDownCounter::Add(long value, const opentelemetry::context::Context &context) noexcept
{
  return storage_->RecordLong(value, context);
}

DoubleUpDownCounter::DoubleUpDownCounter(InstrumentDescriptor instrument_descriptor,
                                         std::unique_ptr<WritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{}

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
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleUpDownCounter::Add(double value) noexcept
{
  auto context = opentelemetry::context::Context{};
  return storage_->RecordDouble(value, context);
}

void DoubleUpDownCounter::Add(double value, const opentelemetry::context::Context &context) noexcept
{
  return storage_->RecordDouble(value, context);
}

LongHistogram::LongHistogram(InstrumentDescriptor instrument_descriptor,
                             std::unique_ptr<WritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{}

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
                                 std::unique_ptr<WritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{}

void DoubleHistogram::Record(double value,
                             const opentelemetry::common::KeyValueIterable &attributes,
                             const opentelemetry::context::Context &context) noexcept
{
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
