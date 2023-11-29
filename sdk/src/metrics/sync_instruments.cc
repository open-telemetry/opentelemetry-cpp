// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/sdk/metrics/sync_instruments.h"

#include <cmath>

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
    OTEL_INTERNAL_LOG_ERROR("[LongCounter::LongCounter] - Error constructing LongCounter."
                            << "The metric storage is invalid for " << instrument_descriptor.name_);
  }
}

void LongCounter::Add(uint64_t value,
                      const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  auto context = opentelemetry::context::Context{};
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongCounter::Add(V,A)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordLong(value, attributes, context);
}

void LongCounter::Add(uint64_t value,
                      const opentelemetry::common::KeyValueIterable &attributes,
                      const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongCounter::Add(V,A,C)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordLong(value, attributes, context);
}

void LongCounter::Add(uint64_t value) noexcept
{
  auto context = opentelemetry::context::Context{};
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongCounter::Add(V)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordLong(value, context);
}

void LongCounter::Add(uint64_t value, const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongCounter::Add(V,C)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
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
    OTEL_INTERNAL_LOG_ERROR("[DoubleCounter::DoubleCounter] - Error constructing DoubleCounter."
                            << "The metric storage is invalid for " << instrument_descriptor.name_);
  }
}

void DoubleCounter::Add(double value,
                        const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  if (value < 0)
  {
    OTEL_INTERNAL_LOG_WARN("[DoubleCounter::Add(V,A)] Value not recorded - negative value for: "
                           << instrument_descriptor_.name_);
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[DoubleCounter::Add(V,A)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  auto context = opentelemetry::context::Context{};
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleCounter::Add(double value,
                        const opentelemetry::common::KeyValueIterable &attributes,
                        const opentelemetry::context::Context &context) noexcept
{
  if (value < 0)
  {
    OTEL_INTERNAL_LOG_WARN("[DoubleCounter::Add(V,A,C)] Value not recorded - negative value for: "
                           << instrument_descriptor_.name_);
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[DoubleCounter::Add(V,A,C)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleCounter::Add(double value) noexcept
{
  if (value < 0)
  {
    OTEL_INTERNAL_LOG_WARN("[DoubleCounter::Add(V)] Value not recorded - negative value for: "
                           << instrument_descriptor_.name_);
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[DoubleCounter::Add(V)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  auto context = opentelemetry::context::Context{};
  return storage_->RecordDouble(value, context);
}

void DoubleCounter::Add(double value, const opentelemetry::context::Context &context) noexcept
{
  if (value < 0)
  {
    OTEL_INTERNAL_LOG_WARN("[DoubleCounter::Add(V)] Value not recorded - negative value for: "
                           << instrument_descriptor_.name_);
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[DoubleCounter::Add(V,C)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
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
        "[LongUpDownCounter::LongUpDownCounter] - Error constructing LongUpDownCounter."
        << "The metric storage is invalid for " << instrument_descriptor.name_);
  }
}

void LongUpDownCounter::Add(int64_t value,
                            const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  auto context = opentelemetry::context::Context{};
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[LongUpDownCounter::Add(V,A)] Value not recorded - invalid storage for: "
        << instrument_descriptor_.name_);
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
    OTEL_INTERNAL_LOG_WARN(
        "[LongUpDownCounter::Add(V,A,C)] Value not recorded - invalid storage for: "
        << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordLong(value, attributes, context);
}

void LongUpDownCounter::Add(int64_t value) noexcept
{
  auto context = opentelemetry::context::Context{};
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongUpDownCounter::Add(V)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordLong(value, context);
}

void LongUpDownCounter::Add(int64_t value, const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[LongUpDownCounter::Add(V,C)] Value not recorded - invalid storage for: "
        << instrument_descriptor_.name_);
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
        "[DoubleUpDownCounter::DoubleUpDownCounter] - Error constructing DoubleUpDownCounter."
        << "The metric storage is invalid for " << instrument_descriptor.name_);
  }
}

void DoubleUpDownCounter::Add(double value,
                              const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[DoubleUpDownCounter::Add(V,A)] Value not recorded - invalid storage for: "
        << instrument_descriptor_.name_);
  }
  auto context = opentelemetry::context::Context{};
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleUpDownCounter::Add(double value,
                              const opentelemetry::common::KeyValueIterable &attributes,
                              const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[DoubleUpDownCounter::Add(V,A,C)] Value not recorded - invalid storage for: "
        << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleUpDownCounter::Add(double value) noexcept
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[DoubleUpDownCounter::Add(V)] Value not recorded - invalid storage for: "
        << instrument_descriptor_.name_);
    return;
  }
  auto context = opentelemetry::context::Context{};
  return storage_->RecordDouble(value, context);
}

void DoubleUpDownCounter::Add(double value, const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[DoubleUpDownCounter::Add(V,C)] Value not recorded - invalid storage for: "
        << instrument_descriptor_.name_);
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
    OTEL_INTERNAL_LOG_ERROR("[LongHistogram::LongHistogram] - Error constructing LongHistogram."
                            << "The metric storage is invalid for " << instrument_descriptor.name_);
  }
}

void LongHistogram::Record(uint64_t value,
                           const opentelemetry::common::KeyValueIterable &attributes,
                           const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[LongHistogram::Record(V,A,C)] Value not recorded - invalid storage for: "
        << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordLong(value, attributes, context);
}

void LongHistogram::Record(uint64_t value, const opentelemetry::context::Context &context) noexcept
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongHistogram::Record(V,C)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordLong(value, context);
}

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
void LongHistogram::Record(uint64_t value,
                           const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongHistogram::Record(V,A)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  auto context = opentelemetry::context::Context{};
  return storage_->RecordLong(value, attributes, context);
}

void LongHistogram::Record(uint64_t value) noexcept
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongHistogram::Record(V)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  auto context = opentelemetry::context::Context{};
  return storage_->RecordLong(value, context);
}
#endif

DoubleHistogram::DoubleHistogram(InstrumentDescriptor instrument_descriptor,
                                 std::unique_ptr<SyncWritableMetricStorage> storage)
    : Synchronous(instrument_descriptor, std::move(storage))
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_ERROR(
        "[DoubleHistogram::DoubleHistogram] - Error constructing DoubleHistogram."
        << "The metric storage is invalid for " << instrument_descriptor.name_);
  }
}

void DoubleHistogram::Record(double value,
                             const opentelemetry::common::KeyValueIterable &attributes,
                             const opentelemetry::context::Context &context) noexcept
{
  if (value < 0)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[DoubleHistogram::Record(V,A,C)] Value not recorded - negative value for: "
        << instrument_descriptor_.name_);
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[DoubleHistogram::Record(V,A,C)] Value not recorded - invalid storage for: "
        << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleHistogram::Record(double value, const opentelemetry::context::Context &context) noexcept
{
  if (value < 0)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[DoubleHistogram::Record(V,C)] Value not recorded - negative value for: "
        << instrument_descriptor_.name_);
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[DoubleHistogram::Record(V,C)] Value not recorded - invalid storage for: "
        << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordDouble(value, context);
}

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
void DoubleHistogram::Record(double value,
                             const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  if (value < 0)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[DoubleHistogram::Record(V,A)] Value not recorded - negative value for: "
        << instrument_descriptor_.name_);
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[DoubleHistogram::Record(V,A)] Value not recorded - invalid storage for: "
        << instrument_descriptor_.name_);
    return;
  }
  auto context = opentelemetry::context::Context{};
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleHistogram::Record(double value) noexcept
{
  if (value < 0)
  {
    OTEL_INTERNAL_LOG_WARN("[DoubleHistogram::Record(V)] Value not recorded - negative value for: "
                           << instrument_descriptor_.name_);
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[DoubleHistogram::Record(V)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  auto context = opentelemetry::context::Context{};
  return storage_->RecordDouble(value, context);
}
#endif

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
