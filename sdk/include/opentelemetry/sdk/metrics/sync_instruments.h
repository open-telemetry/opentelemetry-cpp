// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/metrics/sync_instruments.h"
#include "opentelemetry/sdk/metrics/instruments.h"
#include "opentelemetry/sdk/metrics/state/metric_storage.h"
#include "opentelemetry/sdk_config.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

// forward declaration
class SyncWritableMetricStorage;

class Synchronous
{
public:
  Synchronous(InstrumentDescriptor instrument_descriptor,
              std::unique_ptr<SyncWritableMetricStorage> storage)
      : instrument_descriptor_(instrument_descriptor), storage_(std::move(storage))
  {}

protected:
  InstrumentDescriptor instrument_descriptor_;
  std::unique_ptr<SyncWritableMetricStorage> storage_;
};

class LongCounter : public Synchronous, public opentelemetry::metrics::Counter<uint64_t>
{
public:
  LongCounter(InstrumentDescriptor instrument_descriptor,
              std::unique_ptr<SyncWritableMetricStorage> storage);

  void Add(uint64_t value,
           const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void Add(uint64_t value,
           const opentelemetry::common::KeyValueIterable &attributes,
           const opentelemetry::context::Context &context) noexcept override;

  void Add(uint64_t value) noexcept override;

  void Add(uint64_t value, const opentelemetry::context::Context &context) noexcept override;
};

class DoubleCounter : public Synchronous, public opentelemetry::metrics::Counter<double>
{

public:
  DoubleCounter(InstrumentDescriptor instrument_descriptor,
                std::unique_ptr<SyncWritableMetricStorage> storage);

  void Add(double value,
           const opentelemetry::common::KeyValueIterable &attributes) noexcept override;
  void Add(double value,
           const opentelemetry::common::KeyValueIterable &attributes,
           const opentelemetry::context::Context &context) noexcept override;

  void Add(double value) noexcept override;
  void Add(double value, const opentelemetry::context::Context &context) noexcept override;
};

class LongUpDownCounter : public Synchronous, public opentelemetry::metrics::UpDownCounter<int64_t>
{
public:
  LongUpDownCounter(InstrumentDescriptor instrument_descriptor,
                    std::unique_ptr<SyncWritableMetricStorage> storage);

  void Add(int64_t value,
           const opentelemetry::common::KeyValueIterable &attributes) noexcept override;
  void Add(int64_t value,
           const opentelemetry::common::KeyValueIterable &attributes,
           const opentelemetry::context::Context &context) noexcept override;

  void Add(int64_t value) noexcept override;
  void Add(int64_t value, const opentelemetry::context::Context &context) noexcept override;
};

class DoubleUpDownCounter : public Synchronous, public opentelemetry::metrics::UpDownCounter<double>
{
public:
  DoubleUpDownCounter(InstrumentDescriptor instrument_descriptor,
                      std::unique_ptr<SyncWritableMetricStorage> storage);

  void Add(double value,
           const opentelemetry::common::KeyValueIterable &attributes) noexcept override;
  void Add(double value,
           const opentelemetry::common::KeyValueIterable &attributes,
           const opentelemetry::context::Context &context) noexcept override;

  void Add(double value) noexcept override;
  void Add(double value, const opentelemetry::context::Context &context) noexcept override;
};

class LongHistogram : public Synchronous, public opentelemetry::metrics::Histogram<uint64_t>
{
public:
  LongHistogram(InstrumentDescriptor instrument_descriptor,
                std::unique_ptr<SyncWritableMetricStorage> storage);

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  void Record(uint64_t value,
              const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void Record(uint64_t value) noexcept override;
#endif

  void Record(uint64_t value,
              const opentelemetry::common::KeyValueIterable &attributes,
              const opentelemetry::context::Context &context) noexcept override;

  void Record(uint64_t value, const opentelemetry::context::Context &context) noexcept override;
};

class DoubleHistogram : public Synchronous, public opentelemetry::metrics::Histogram<double>
{
public:
  DoubleHistogram(InstrumentDescriptor instrument_descriptor,
                  std::unique_ptr<SyncWritableMetricStorage> storage);

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
  void Record(double value,
              const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void Record(double value) noexcept override;
#endif

  void Record(double value,
              const opentelemetry::common::KeyValueIterable &attributes,
              const opentelemetry::context::Context &context) noexcept override;

  void Record(double value, const opentelemetry::context::Context &context) noexcept override;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
