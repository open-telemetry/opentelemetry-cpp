// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

#ifdef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW
#  include "opentelemetry/metrics/sync_instruments.h"
#endif

#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/sdk/common/attribute_utils.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/meter_enabled_state.h"
#include "opentelemetry/sdk/metrics/state/metric_storage.h"
#include "opentelemetry/sdk/metrics/sync_instruments.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
namespace
{

bool ToInt64Value(uint64_t value, const char *operation, int64_t &converted) noexcept
{
  if (value > static_cast<uint64_t>(std::numeric_limits<int64_t>::max()))
  {
    OTEL_INTERNAL_LOG_WARN(operation << " Value not recorded - value exceeds int64_t max");
    return false;
  }
  converted = static_cast<int64_t>(value);
  return true;
}

}  // namespace

LongCounter::LongCounter(const InstrumentDescriptor &instrument_descriptor,
                         std::unique_ptr<SyncWritableMetricStorage> storage,
                         std::shared_ptr<MeterEnabledState> meter_enabled_state)
    : Synchronous(instrument_descriptor, std::move(storage), std::move(meter_enabled_state))
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
  if (!IsEnabled())
  {
    return;
  }
  auto context = opentelemetry::context::Context{};
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongCounter::Add(V,A)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  int64_t converted = 0;
  if (ToInt64Value(value, "[LongCounter::Add(V,A)]", converted))
  {
    return storage_->RecordLong(converted, attributes, context);
  }
}

void LongCounter::Add(uint64_t value,
                      const opentelemetry::common::KeyValueIterable &attributes,
                      const opentelemetry::context::Context &context) noexcept
{
  if (!IsEnabled())
  {
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongCounter::Add(V,A,C)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  int64_t converted = 0;
  if (ToInt64Value(value, "[LongCounter::Add(V,A,C)]", converted))
  {
    return storage_->RecordLong(converted, attributes, context);
  }
}

void LongCounter::Add(uint64_t value) noexcept
{
  if (!IsEnabled())
  {
    return;
  }
  auto context = opentelemetry::context::Context{};
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongCounter::Add(V)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  int64_t converted = 0;
  if (ToInt64Value(value, "[LongCounter::Add(V)]", converted))
  {
    return storage_->RecordLong(converted, context);
  }
}

void LongCounter::Add(uint64_t value, const opentelemetry::context::Context &context) noexcept
{
  if (!IsEnabled())
  {
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongCounter::Add(V,C)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  int64_t converted = 0;
  if (ToInt64Value(value, "[LongCounter::Add(V,C)]", converted))
  {
    return storage_->RecordLong(converted, context);
  }
}

DoubleCounter::DoubleCounter(const InstrumentDescriptor &instrument_descriptor,
                             std::unique_ptr<SyncWritableMetricStorage> storage,
                             std::shared_ptr<MeterEnabledState> meter_enabled_state)
    : Synchronous(instrument_descriptor, std::move(storage), std::move(meter_enabled_state))
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
  if (!IsEnabled())
  {
    return;
  }
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
  if (!IsEnabled())
  {
    return;
  }
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
  if (!IsEnabled())
  {
    return;
  }
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
  if (!IsEnabled())
  {
    return;
  }
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

LongUpDownCounter::LongUpDownCounter(const InstrumentDescriptor &instrument_descriptor,
                                     std::unique_ptr<SyncWritableMetricStorage> storage,
                                     std::shared_ptr<MeterEnabledState> meter_enabled_state)
    : Synchronous(instrument_descriptor, std::move(storage), std::move(meter_enabled_state))
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
  if (!IsEnabled())
  {
    return;
  }
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
  if (!IsEnabled())
  {
    return;
  }
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
  if (!IsEnabled())
  {
    return;
  }
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
  if (!IsEnabled())
  {
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[LongUpDownCounter::Add(V,C)] Value not recorded - invalid storage for: "
        << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordLong(value, context);
}

DoubleUpDownCounter::DoubleUpDownCounter(const InstrumentDescriptor &instrument_descriptor,
                                         std::unique_ptr<SyncWritableMetricStorage> storage,
                                         std::shared_ptr<MeterEnabledState> meter_enabled_state)
    : Synchronous(instrument_descriptor, std::move(storage), std::move(meter_enabled_state))
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
  if (!IsEnabled())
  {
    return;
  }
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
  if (!IsEnabled())
  {
    return;
  }
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
  if (!IsEnabled())
  {
    return;
  }
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
  if (!IsEnabled())
  {
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[DoubleUpDownCounter::Add(V,C)] Value not recorded - invalid storage for: "
        << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordDouble(value, context);
}

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
LongGauge::LongGauge(const InstrumentDescriptor &instrument_descriptor,
                     std::unique_ptr<SyncWritableMetricStorage> storage,
                     std::shared_ptr<MeterEnabledState> meter_enabled_state)
    : Synchronous(instrument_descriptor, std::move(storage), std::move(meter_enabled_state))
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_ERROR("[LongGauge::LongGauge] - Error constructing LongGauge."
                            << "The metric storage is invalid for " << instrument_descriptor.name_);
  }
}

void LongGauge::Record(int64_t value,
                       const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  if (!IsEnabled())
  {
    return;
  }
  auto context = opentelemetry::context::Context{};
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongGauge::Record(V,A)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordLong(value, attributes, context);
}

void LongGauge::Record(int64_t value,
                       const opentelemetry::common::KeyValueIterable &attributes,
                       const opentelemetry::context::Context &context) noexcept
{
  if (!IsEnabled())
  {
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongGauge::Record(V,A,C)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordLong(value, attributes, context);
}

void LongGauge::Record(int64_t value) noexcept
{
  if (!IsEnabled())
  {
    return;
  }
  auto context = opentelemetry::context::Context{};
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongGauge::Record(V)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordLong(value, context);
}

void LongGauge::Record(int64_t value, const opentelemetry::context::Context &context) noexcept
{
  if (!IsEnabled())
  {
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongGauge::Record(V,C)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordLong(value, context);
}

DoubleGauge::DoubleGauge(const InstrumentDescriptor &instrument_descriptor,
                         std::unique_ptr<SyncWritableMetricStorage> storage,
                         std::shared_ptr<MeterEnabledState> meter_enabled_state)
    : Synchronous(instrument_descriptor, std::move(storage), std::move(meter_enabled_state))
{
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_ERROR("[DoubleGauge::DoubleGauge] - Error constructing DoubleUpDownCounter."
                            << "The metric storage is invalid for " << instrument_descriptor.name_);
  }
}

void DoubleGauge::Record(double value,
                         const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  if (!IsEnabled())
  {
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[DoubleGauge::Record(V,A)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
  }
  auto context = opentelemetry::context::Context{};
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleGauge::Record(double value,
                         const opentelemetry::common::KeyValueIterable &attributes,
                         const opentelemetry::context::Context &context) noexcept
{
  if (!IsEnabled())
  {
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[DoubleGauge::Record(V,A,C)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordDouble(value, attributes, context);
}

void DoubleGauge::Record(double value) noexcept
{
  if (!IsEnabled())
  {
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[DoubleGauge::Record(V)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  auto context = opentelemetry::context::Context{};
  return storage_->RecordDouble(value, context);
}

void DoubleGauge::Record(double value, const opentelemetry::context::Context &context) noexcept
{
  if (!IsEnabled())
  {
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[DoubleGauge::Record(V,C)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  return storage_->RecordDouble(value, context);
}
#endif

LongHistogram::LongHistogram(const InstrumentDescriptor &instrument_descriptor,
                             std::unique_ptr<SyncWritableMetricStorage> storage,
                             std::shared_ptr<MeterEnabledState> meter_enabled_state)
    : Synchronous(instrument_descriptor, std::move(storage), std::move(meter_enabled_state))
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
  if (!IsEnabled())
  {
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN(
        "[LongHistogram::Record(V,A,C)] Value not recorded - invalid storage for: "
        << instrument_descriptor_.name_);
    return;
  }
  int64_t converted = 0;
  if (ToInt64Value(value, "[LongHistogram::Record(V,A,C)]", converted))
  {
    return storage_->RecordLong(converted, attributes, context);
  }
}

void LongHistogram::Record(uint64_t value, const opentelemetry::context::Context &context) noexcept
{
  if (!IsEnabled())
  {
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongHistogram::Record(V,C)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  int64_t converted = 0;
  if (ToInt64Value(value, "[LongHistogram::Record(V,C)]", converted))
  {
    return storage_->RecordLong(converted, context);
  }
}

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
void LongHistogram::Record(uint64_t value,
                           const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  if (!IsEnabled())
  {
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongHistogram::Record(V,A)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  auto context      = opentelemetry::context::Context{};
  int64_t converted = 0;
  if (ToInt64Value(value, "[LongHistogram::Record(V,A)]", converted))
  {
    return storage_->RecordLong(converted, attributes, context);
  }
}

void LongHistogram::Record(uint64_t value) noexcept
{
  if (!IsEnabled())
  {
    return;
  }
  if (!storage_)
  {
    OTEL_INTERNAL_LOG_WARN("[LongHistogram::Record(V)] Value not recorded - invalid storage for: "
                           << instrument_descriptor_.name_);
    return;
  }
  auto context      = opentelemetry::context::Context{};
  int64_t converted = 0;
  if (ToInt64Value(value, "[LongHistogram::Record(V)]", converted))
  {
    return storage_->RecordLong(converted, context);
  }
}
#endif

DoubleHistogram::DoubleHistogram(const InstrumentDescriptor &instrument_descriptor,
                                 std::unique_ptr<SyncWritableMetricStorage> storage,
                                 std::shared_ptr<MeterEnabledState> meter_enabled_state)
    : Synchronous(instrument_descriptor, std::move(storage), std::move(meter_enabled_state))
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
  if (!IsEnabled())
  {
    return;
  }
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
  if (!IsEnabled())
  {
    return;
  }
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
  if (!IsEnabled())
  {
    return;
  }
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
  if (!IsEnabled())
  {
    return;
  }
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

#ifdef OPENTELEMETRY_HAVE_METRICS_BOUND_INSTRUMENTS_PREVIEW
namespace
{

// Owns a copy of an attribute set so a deferred bind can replay it. AttributeValue's
// string_view and span alternatives point at caller memory, so values are stored owned.
class OwnedAttributesIterable final : public opentelemetry::common::KeyValueIterable
{
public:
  explicit OwnedAttributesIterable(const opentelemetry::common::KeyValueIterable &attributes)
  {
    entries_.reserve(attributes.size());
    attributes.ForEachKeyValue([this](nostd::string_view key,
                                      opentelemetry::common::AttributeValue value) {
      entries_.emplace_back(std::string{key.data(), key.size()},
                            nostd::visit(opentelemetry::sdk::common::AttributeConverter{}, value));
      return true;
    });
  }

  bool ForEachKeyValue(
      nostd::function_ref<bool(nostd::string_view, opentelemetry::common::AttributeValue)> callback)
      const noexcept override
  {
    for (const auto &entry : entries_)
    {
      if (!callback(entry.first, ToAttributeValue(entry.second)))
      {
        return false;
      }
    }
    return true;
  }

  size_t size() const noexcept override { return entries_.size(); }

private:
  // Borrows from the owned value. get_if rather than a visitor so this cannot throw.
  static opentelemetry::common::AttributeValue ToAttributeValue(
      const opentelemetry::sdk::common::OwnedAttributeValue &owned) noexcept
  {
    if (const auto *v = nostd::get_if<bool>(&owned))
    {
      return *v;
    }
    if (const auto *v = nostd::get_if<int32_t>(&owned))
    {
      return *v;
    }
    if (const auto *v = nostd::get_if<uint32_t>(&owned))
    {
      return *v;
    }
    if (const auto *v = nostd::get_if<int64_t>(&owned))
    {
      return *v;
    }
    if (const auto *v = nostd::get_if<uint64_t>(&owned))
    {
      return *v;
    }
    if (const auto *v = nostd::get_if<double>(&owned))
    {
      return *v;
    }
    if (const auto *v = nostd::get_if<std::string>(&owned))
    {
      return nostd::string_view{*v};
    }
    if (const auto *v = nostd::get_if<std::vector<int32_t>>(&owned))
    {
      return nostd::span<const int32_t>{v->data(), v->size()};
    }
    if (const auto *v = nostd::get_if<std::vector<uint32_t>>(&owned))
    {
      return nostd::span<const uint32_t>{v->data(), v->size()};
    }
    if (const auto *v = nostd::get_if<std::vector<int64_t>>(&owned))
    {
      return nostd::span<const int64_t>{v->data(), v->size()};
    }
    if (const auto *v = nostd::get_if<std::vector<uint64_t>>(&owned))
    {
      return nostd::span<const uint64_t>{v->data(), v->size()};
    }
    if (const auto *v = nostd::get_if<std::vector<double>>(&owned))
    {
      return nostd::span<const double>{v->data(), v->size()};
    }
    if (const auto *v = nostd::get_if<std::vector<uint8_t>>(&owned))
    {
      return nostd::span<const uint8_t>{v->data(), v->size()};
    }
    // vector<bool> is bit-packed and a string_view span cannot borrow from vector<string>,
    // so neither can be replayed by borrowing.
    return nostd::string_view{};
  }

  std::vector<std::pair<std::string, opentelemetry::sdk::common::OwnedAttributeValue>> entries_;
};

// Defers Bind until the Meter is enabled, so a handle taken while disabled starts working
// instead of staying dead. Storage that refuses while enabled cannot bind at all, so that
// case is latched.
class LazyBoundEntry
{
public:
  LazyBoundEntry(SyncWritableMetricStorage *storage,
                 const opentelemetry::common::KeyValueIterable &attributes,
                 std::shared_ptr<BoundSyncWritableMetricStorage> bound)
      : storage_(storage), bound_(std::move(bound))
  {
    if (storage_ == nullptr)
    {
      unbindable_ = true;
    }
    else if (!bound_)
    {
      attributes_ = std::make_unique<OwnedAttributesIterable>(attributes);
    }
  }

  // Binds on first use if needed. Only call once the Meter is known to be enabled.
  BoundSyncWritableMetricStorage *Get() noexcept
  {
    const std::lock_guard<std::mutex> guard(lock_);
    if (bound_)
    {
      return bound_.get();
    }
    if (unbindable_ || !attributes_)
    {
      return nullptr;
    }
    bound_ = storage_->Bind(*attributes_);
    if (!bound_)
    {
      // Enabled and still refused: binding is unsupported here.
      unbindable_ = true;
      return nullptr;
    }
    attributes_.reset();
    return bound_.get();
  }

private:
  SyncWritableMetricStorage *storage_;
  std::unique_ptr<OwnedAttributesIterable> attributes_;
  std::mutex lock_;
  std::shared_ptr<BoundSyncWritableMetricStorage> bound_;
  bool unbindable_ = false;
};

class BoundLongCounterImpl : public opentelemetry::metrics::BoundCounter<uint64_t>
{
public:
  BoundLongCounterImpl(SyncWritableMetricStorage *storage,
                       const opentelemetry::common::KeyValueIterable &attributes,
                       std::shared_ptr<BoundSyncWritableMetricStorage> bound,
                       std::shared_ptr<MeterEnabledState> meter_enabled_state) noexcept
      : entry_(storage, attributes, std::move(bound)),
        meter_enabled_state_(std::move(meter_enabled_state))
  {}
  void Add(uint64_t value) noexcept override
  {
    if (!meter_enabled_state_->IsEnabled())
    {
      return;
    }
    auto *entry = entry_.Get();
    if (entry == nullptr)
    {
      return;
    }
    int64_t converted = 0;
    if (ToInt64Value(value, "[BoundLongCounter::Add(V)]", converted))
    {
      entry->RecordLong(converted);
    }
  }

private:
  LazyBoundEntry entry_;
  std::shared_ptr<MeterEnabledState> meter_enabled_state_;
};

class BoundDoubleCounterImpl : public opentelemetry::metrics::BoundCounter<double>
{
public:
  BoundDoubleCounterImpl(SyncWritableMetricStorage *storage,
                         const opentelemetry::common::KeyValueIterable &attributes,
                         std::shared_ptr<BoundSyncWritableMetricStorage> bound,
                         std::shared_ptr<MeterEnabledState> meter_enabled_state) noexcept
      : entry_(storage, attributes, std::move(bound)),
        meter_enabled_state_(std::move(meter_enabled_state))
  {}
  void Add(double value) noexcept override
  {
    if (!meter_enabled_state_->IsEnabled())
    {
      return;
    }
    if (value < 0)
    {
      OTEL_INTERNAL_LOG_WARN("[BoundDoubleCounter::Add(V)] Value not recorded - negative value");
      return;
    }
    auto *entry = entry_.Get();
    if (entry == nullptr)
    {
      return;
    }
    entry->RecordDouble(value);
  }

private:
  LazyBoundEntry entry_;
  std::shared_ptr<MeterEnabledState> meter_enabled_state_;
};

class BoundLongHistogramImpl : public opentelemetry::metrics::BoundHistogram<uint64_t>
{
public:
  BoundLongHistogramImpl(SyncWritableMetricStorage *storage,
                         const opentelemetry::common::KeyValueIterable &attributes,
                         std::shared_ptr<BoundSyncWritableMetricStorage> bound,
                         std::shared_ptr<MeterEnabledState> meter_enabled_state) noexcept
      : entry_(storage, attributes, std::move(bound)),
        meter_enabled_state_(std::move(meter_enabled_state))
  {}
  void Record(uint64_t value) noexcept override
  {
    if (!meter_enabled_state_->IsEnabled())
    {
      return;
    }
    auto *entry = entry_.Get();
    if (entry == nullptr)
    {
      return;
    }
    int64_t converted = 0;
    if (ToInt64Value(value, "[BoundLongHistogram::Record(V)]", converted))
    {
      entry->RecordLong(converted);
    }
  }

private:
  LazyBoundEntry entry_;
  std::shared_ptr<MeterEnabledState> meter_enabled_state_;
};

class BoundDoubleHistogramImpl : public opentelemetry::metrics::BoundHistogram<double>
{
public:
  BoundDoubleHistogramImpl(SyncWritableMetricStorage *storage,
                           const opentelemetry::common::KeyValueIterable &attributes,
                           std::shared_ptr<BoundSyncWritableMetricStorage> bound,
                           std::shared_ptr<MeterEnabledState> meter_enabled_state) noexcept
      : entry_(storage, attributes, std::move(bound)),
        meter_enabled_state_(std::move(meter_enabled_state))
  {}
  void Record(double value) noexcept override
  {
    if (!meter_enabled_state_->IsEnabled())
    {
      return;
    }
    if (value < 0)
    {
      OTEL_INTERNAL_LOG_WARN(
          "[BoundDoubleHistogram::Record(V)] Value not recorded - negative value");
      return;
    }
    auto *entry = entry_.Get();
    if (entry == nullptr)
    {
      return;
    }
    entry->RecordDouble(value);
  }

private:
  LazyBoundEntry entry_;
  std::shared_ptr<MeterEnabledState> meter_enabled_state_;
};

}  // namespace

opentelemetry::nostd::unique_ptr<opentelemetry::metrics::BoundCounter<uint64_t>> LongCounter::Bind(
    const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  std::shared_ptr<BoundSyncWritableMetricStorage> bound;
  if (storage_)
  {
    bound = storage_->Bind(attributes);
  }
  // A null entry is not final: the handle binds later, once the Meter is enabled.
  return opentelemetry::nostd::unique_ptr<opentelemetry::metrics::BoundCounter<uint64_t>>{
      new BoundLongCounterImpl(storage_.get(), attributes, std::move(bound), meter_enabled_state_)};
}

opentelemetry::nostd::unique_ptr<opentelemetry::metrics::BoundCounter<double>> DoubleCounter::Bind(
    const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  std::shared_ptr<BoundSyncWritableMetricStorage> bound;
  if (storage_)
  {
    bound = storage_->Bind(attributes);
  }
  return opentelemetry::nostd::unique_ptr<opentelemetry::metrics::BoundCounter<double>>{
      new BoundDoubleCounterImpl(storage_.get(), attributes, std::move(bound),
                                 meter_enabled_state_)};
}

opentelemetry::nostd::unique_ptr<opentelemetry::metrics::BoundHistogram<uint64_t>>
LongHistogram::Bind(const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  std::shared_ptr<BoundSyncWritableMetricStorage> bound;
  if (storage_)
  {
    bound = storage_->Bind(attributes);
  }
  return opentelemetry::nostd::unique_ptr<opentelemetry::metrics::BoundHistogram<uint64_t>>{
      new BoundLongHistogramImpl(storage_.get(), attributes, std::move(bound),
                                 meter_enabled_state_)};
}

opentelemetry::nostd::unique_ptr<opentelemetry::metrics::BoundHistogram<double>>
DoubleHistogram::Bind(const opentelemetry::common::KeyValueIterable &attributes) noexcept
{
  std::shared_ptr<BoundSyncWritableMetricStorage> bound;
  if (storage_)
  {
    bound = storage_->Bind(attributes);
  }
  return opentelemetry::nostd::unique_ptr<opentelemetry::metrics::BoundHistogram<double>>{
      new BoundDoubleHistogramImpl(storage_.get(), attributes, std::move(bound),
                                   meter_enabled_state_)};
}
#endif

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
