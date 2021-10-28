// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <map>
#include "opentelemetry/metrics_new/noop.h"

TEST(Counter, Add)
{
  std::shared_ptr<opentelemetry::metrics_new::Counter<long>> counter{
      new opentelemetry::metrics_new::NoopCounter<long>("test", "none", "unitless")};

  std::map<std::string, std::string> labels = {{"k1", "v1"}};
  EXPECT_NO_THROW(counter->Add(10l, labels));
  EXPECT_NO_THROW(counter->Add(2l));
  EXPECT_NO_THROW(counter->Add(10l, {{"k1", "1"}, {"k2", 2}}));
}

TEST(histogram, Record)
{
  std::shared_ptr<opentelemetry::metrics_new::Histogram<long>> counter{
      new opentelemetry::metrics_new::NoopHistogram<long>("test", "none", "unitless")};

  std::map<std::string, std::string> labels = {{"k1", "v1"}};
  EXPECT_NO_THROW(counter->Record(10l, labels));
  EXPECT_NO_THROW(counter->Record(2l));
  EXPECT_NO_THROW(counter->Record(10l, {{"k1", "1"}, {"k2", 2}}));
}

TEST(UpDownCountr, Record)
{
  std::shared_ptr<opentelemetry::metrics_new::UpDownCounter<long>> counter{
      new opentelemetry::metrics_new::NoopUpDownCounter<long>("test", "none", "unitless")};

  std::map<std::string, std::string> labels = {{"k1", "v1"}};
  EXPECT_NO_THROW(counter->Add(10l, labels));
  EXPECT_NO_THROW(counter->Add(2l));
  EXPECT_NO_THROW(counter->Add(10l, {{"k1", "1"}, {"k2", 2}}));
}