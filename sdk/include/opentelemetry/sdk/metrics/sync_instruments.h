// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/key_value_iterable.h"
#  include "opentelemetry/metrics/sync_instruments.h"
#  include "opentelemetry/nostd/string_view.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class Synchronous
{
public:
  Synchronous(nostd::string_view name,
              InstrumentDescriptor &descriptor,
              nostd::string_view description = "",
              nostd::string_view unit        = "")
      : name_(name), description_(description), unit_(unit)
  {}

  nostd::string_view GetName() { return name_; }

  nostd::string_view GetDescription() { return description_; }

  nostd::string_view GetUnit() { return unit_; }

private:
  std::string name_;
  std::string description_;
  std::string unit_;
  InstrumentDescriptor instrument_descriptor_;
};

class LongCounter : public Synchronous, opentelemetry::metrics::Counter<long>
{
public:
  LongCounter(nostd::string_view name,
              InstrumentDescriptor &descriptor,
              nostd::string_view description = "",
              nostd::string_view unit        = "");

  void Add(long value, const common::KeyValueIterable &attributes) noexcept override;
};

class DoubleCounter : public Synchronous, opentelemetry::metrics::Counter<double>
{

public:
  DoubleCounter(nostd::string_view name,
                InstrumentDescriptor &descriptor,
                nostd::string_view description = "",
                nostd::string_view unit        = "");

  void Add(double value, const common::KeyValueIterable &attributes) noexcept override;
};

class LongUpDownCounter : public Synchronous, opentelemetry::metrics::UpDownCounter<long>
{
public:
  LongUpDownCounter(nostd::string_view name,
                    InstrumentDescriptor &descriptor,
                    nostd::string_view description = "",
                    nostd::string_view unit        = "");

  void Add(long value, const common::KeyValueIterable &attributes) noexcept override;
};

class DoubleUpDownCounter : public Synchronous, opentelemetry::metrics::Counter<double>
{
public:
  DoubleUpDownCounter(nostd::string_view name,
                      InstrumentDescriptor &descriptor,
                      nostd::string_view description = "",
                      nostd::string_view unit        = "");

  void Add(double value, const common::KeyValueIterable &attributes) noexcept override;
};

class LongHistogram : public Synchronous, opentelemetry::metrics::Histogram<long>
{
public:
  LongHistogram(nostd::string_view name,
                InstrumentDescriptor &descriptor,
                nostd::string_view description = "",
                nostd::string_view unit        = "");

  void Record(long value, const common::KeyValueIterable &attributes) noexcept override;
};

class DoubleHistogram : public Synchronous, opentelemetry::metrics::Histogram<double>
{
public:
  DoubleHistogram(nostd::string_view name,
                  InstrumentDescriptor &descriptor,
                  nostd::string_view description = "",
                  nostd::string_view unit        = "");

  void Record(double value, const common::KeyValueIterable &attributes) noexcept override;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif