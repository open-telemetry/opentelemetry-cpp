// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/sync_instruments.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
LongCounter::LongCounter(nostd::string_view name,
                         InstrumentDescriptor &descriptor,
                         nostd::string_view description,
                         nostd::string_view unit)
    : Synchronous(name, descriptor, description, unit)
{}

void LongCounter::Add(long value, const common::KeyValueIterable &attributes) noexcept
{
  // TBD - Push to consumer / writable storage for aggregation
}

DoubleCounter::DoubleCounter(nostd::string_view name,
                             InstrumentDescriptor &descriptor,
                             nostd::string_view description,
                             nostd::string_view unit)
    : Synchronous(name, descriptor, description, unit)
{}

void DoubleCounter::Add(double value, const common::KeyValueIterable &attributes) noexcept
{
  // TBD - Push to consumer / writable storage for aggregation
}

LongUpDownCounter::LongUpDownCounter(nostd::string_view name,
                                     InstrumentDescriptor &descriptor,
                                     nostd::string_view description,
                                     nostd::string_view unit)
    : Synchronous(name, descriptor, description, unit)
{}

void LongUpDownCounter::Add(long value, const common::KeyValueIterable &attributes) noexcept
{
  // TBD - Push to consumer / writable storage for aggregation
}

DoubleUpDownCounter::DoubleUpDownCounter(nostd::string_view name,
                                         InstrumentDescriptor &descriptor,
                                         nostd::string_view description,
                                         nostd::string_view unit)
    : Synchronous(name, descriptor, description, unit)
{}

void DoubleUpDownCounter::Add(double value, const common::KeyValueIterable &attributes) noexcept
{
  // TBD - Push to consumer / writable storage for aggregation
}

LongHistogram::LongHistogram(nostd::string_view name,
                             InstrumentDescriptor &descriptor,
                             nostd::string_view description,
                             nostd::string_view unit)
    : Synchronous(name, descriptor, description, unit)
{}

void LongHistogram::Record(long value, const common::KeyValueIterable &attributes) noexcept
{

  // TBD - Push to consumer / writable storage for aggregation
}

DoubleHistogram::DoubleHistogram(nostd::string_view name,
                                 InstrumentDescriptor &descriptor,
                                 nostd::string_view description,
                                 nostd::string_view unit)
    : Synchronous(name, descriptor, description, unit)
{}

void DoubleHistogram::Record(double value, const common::KeyValueIterable &attributes) noexcept
{

  // TBD - Push to consumer / writable storage for aggregation
}
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
