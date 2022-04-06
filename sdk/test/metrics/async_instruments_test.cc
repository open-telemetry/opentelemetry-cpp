// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/async_instruments.h"

#  include <gtest/gtest.h>

using namespace opentelemetry;
using namespace opentelemetry::sdk::metrics;

using M = std::map<std::string, std::string>;

void asyc_generate_measurements_long(opentelemetry::metrics::ObserverResult<long> &observer) {}

void asyc_generate_measurements_double(opentelemetry::metrics::ObserverResult<double> &observer) {}

TEST(AsyncInstruments, LongObservableCounter)
{
  auto asyc_generate_meas_long = [](opentelemetry::metrics::ObserverResult<long> &observer) {};
  EXPECT_NO_THROW(
      LongObservableCounter counter("long_counter", asyc_generate_meas_long, "description", "1"));
}

TEST(AsyncInstruments, DoubleObservableCounter)
{
  auto asyc_generate_meas_double = [](opentelemetry::metrics::ObserverResult<double> &observer) {};
  EXPECT_NO_THROW(DoubleObservableCounter counter("long_counter", asyc_generate_meas_double,
                                                  "description", "1"));
}

TEST(AsyncInstruments, LongObservableGauge)
{
  auto asyc_generate_meas_long = [](opentelemetry::metrics::ObserverResult<long> &observer) {};
  EXPECT_NO_THROW(
      LongObservableGauge counter("long_counter", asyc_generate_meas_long, "description", "1"));
}

TEST(AsyncInstruments, DoubleObservableGauge)
{
  auto asyc_generate_meas_double = [](opentelemetry::metrics::ObserverResult<double> &observer) {};
  EXPECT_NO_THROW(
      DoubleObservableGauge counter("long_counter", asyc_generate_meas_double, "description", "1"));
}

TEST(AsyncInstruments, LongObservableUpDownCounter)
{
  auto asyc_generate_meas_long = [](opentelemetry::metrics::ObserverResult<long> &observer) {};
  EXPECT_NO_THROW(LongObservableUpDownCounter counter("long_counter", asyc_generate_meas_long,
                                                      "description", "1"));
}

TEST(AsyncInstruments, DoubleObservableUpDownCounter)
{
  auto asyc_generate_meas_double = [](opentelemetry::metrics::ObserverResult<double> &observer) {};
  EXPECT_NO_THROW(DoubleObservableUpDownCounter counter("long_counter", asyc_generate_meas_double,
                                                        "description", "1"));
}

#endif
