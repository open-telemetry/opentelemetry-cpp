// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/common/timestamp.h"
#  include "opentelemetry/nostd/variant.h"
#  include "opentelemetry/sdk/metrics/instruments.h"
#  include "opentelemetry/version.h"

#  include <list>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

using ValueType = nostd::variant<long, double>;
using ListType  = nostd::variant<std::list<long>, std::list<double>>;

// TODO: remove ctors and initializers from below classes when GCC<5 stops shipping on Ubuntu

class SumPointData
{
public:
  // TODO: remove ctors and initializers when GCC<5 stops shipping on Ubuntu
  SumPointData(SumPointData &&)      = default;
  SumPointData(const SumPointData &) = default;
  SumPointData &operator=(SumPointData &&) = default;
  SumPointData()                           = default;

  ValueType value_ = {};
};

class LastValuePointData
{
public:
  // TODO: remove ctors and initializers when GCC<5 stops shipping on Ubuntu
  LastValuePointData(LastValuePointData &&)      = default;
  LastValuePointData(const LastValuePointData &) = default;
  LastValuePointData &operator=(LastValuePointData &&) = default;
  LastValuePointData()                                 = default;

  ValueType value_                                  = {};
  bool is_lastvalue_valid_                          = {};
  opentelemetry::common::SystemTimestamp sample_ts_ = {};
};

class HistogramPointData
{
public:
  // TODO: remove ctors and initializers when GCC<5 stops shipping on Ubuntu
  HistogramPointData(HistogramPointData &&) = default;
  HistogramPointData &operator=(HistogramPointData &&) = default;
  HistogramPointData(const HistogramPointData &)       = default;
  HistogramPointData()                                 = default;

  ListType boundaries_          = {};
  ValueType sum_                = {};
  std::vector<uint64_t> counts_ = {};
  uint64_t count_               = {};
};

class DropPointData
{
public:
  // TODO: remove ctors and initializers when GCC<5 stops shipping on Ubuntu
  DropPointData(DropPointData &&)      = default;
  DropPointData(const DropPointData &) = default;
  DropPointData()                      = default;
  DropPointData &operator=(DropPointData &&) = default;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
