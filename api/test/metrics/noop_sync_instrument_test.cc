// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW

#  include <gtest/gtest.h>
#  include <map>
#  include "opentelemetry/metrics/noop.h"

TEST(Counter, Add)
{
  std::shared_ptr<opentelemetry::metrics::Counter<long>> counter{
      new opentelemetry::metrics::NoopCounter<long>("test", "none", "unitless")};

  std::map<std::string, std::string> labels = {{"k1", "v1"}};
  counter->Add(10l, labels);
  counter->Add(10l, labels, opentelemetry::context::Context{});
  counter->Add(2l);
  counter->Add(2l, opentelemetry::context::Context{});
  counter->Add(10l, {{"k1", "1"}, {"k2", 2}});
  counter->Add(10l, {{"k1", "1"}, {"k2", 2}}, opentelemetry::context::Context{});
}

TEST(histogram, Record)
{
  std::shared_ptr<opentelemetry::metrics::Histogram<long>> counter{
      new opentelemetry::metrics::NoopHistogram<long>("test", "none", "unitless")};

  std::map<std::string, std::string> labels = {{"k1", "v1"}};
  counter->Record(10l, labels, opentelemetry::context::Context{});
  counter->Record(2l, opentelemetry::context::Context{});

  counter->Record(10l, {{"k1", "1"}, {"k2", 2}}, opentelemetry::context::Context{});
}

TEST(UpDownCountr, Record)
{
  std::shared_ptr<opentelemetry::metrics::UpDownCounter<long>> counter{
      new opentelemetry::metrics::NoopUpDownCounter<long>("test", "none", "unitless")};

  std::map<std::string, std::string> labels = {{"k1", "v1"}};
  counter->Add(10l, labels);
  counter->Add(10l, labels, opentelemetry::context::Context{});
  counter->Add(2l);
  counter->Add(2l, opentelemetry::context::Context{});
  counter->Add(10l, {{"k1", "1"}, {"k2", 2}});
  counter->Add(10l, {{"k1", "1"}, {"k2", 2}}, opentelemetry::context::Context{});
}

#endif