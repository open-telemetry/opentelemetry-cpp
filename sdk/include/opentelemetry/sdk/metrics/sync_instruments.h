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
  Synchronous(InstrumentDescriptor instrument_descriptor,
              std::unique_ptr<WritableMetricStorage> storage)
      : instrument_descriptor_(instrument_descriptor), storage_(std::move(storage))
  {}

protected:
  InstrumentDescriptor instrument_descriptor_;
  std::unique_ptr<WritableMetricStorage> storage_;
};

class LongCounter : public Synchronous, public opentelemetry::metrics::Counter<long>
{
public:
  LongCounter(InstrumentDescriptor instrument_descriptor,
              std::unique_ptr<WritableMetricStorage> storage);

  void Add(long value, const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void Add(long value) noexcept override;
};

class DoubleCounter : public Synchronous, public opentelemetry::metrics::Counter<double>
{

public:
  DoubleCounter(InstrumentDescriptor instrument_descriptor,
                std::unique_ptr<WritableMetricStorage> storage);

  void Add(double value,
           const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void Add(double value) noexcept override;
};

class LongUpDownCounter : public Synchronous, public opentelemetry::metrics::UpDownCounter<long>
{
public:
  LongUpDownCounter(InstrumentDescriptor instrument_descriptor,
                    std::unique_ptr<WritableMetricStorage> storage);

  void Add(long value, const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void Add(long value) noexcept override;
};

class DoubleUpDownCounter : public Synchronous, public opentelemetry::metrics::UpDownCounter<double>
{
public:
  DoubleUpDownCounter(InstrumentDescriptor instrument_descriptor,
                      std::unique_ptr<WritableMetricStorage> storage);

  void Add(double value,
           const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void Add(double value) noexcept override;
};

class LongHistogram : public Synchronous, public opentelemetry::metrics::Histogram<long>
{
public:
  LongHistogram(InstrumentDescriptor instrument_descriptor,
                std::unique_ptr<WritableMetricStorage> storage);

  void Record(long value,
              const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void Record(long value) noexcept override;
};

class DoubleHistogram : public Synchronous, public opentelemetry::metrics::Histogram<double>
{
public:
  DoubleHistogram(InstrumentDescriptor instrument_descriptor,
                  std::unique_ptr<WritableMetricStorage> storage);

  void Record(double value,
              const opentelemetry::common::KeyValueIterable &attributes) noexcept override;

  void Record(double value) noexcept override;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif