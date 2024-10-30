// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <stdint.h>
#include <map>
#include <memory>
#include <string>

#include "opentelemetry/context/context.h"
#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/metrics/sync_instruments.h"
#include "opentelemetry/nostd/string_view.h"

TEST(Counter, Add)
{
  std::shared_ptr<opentelemetry::metrics::Counter<uint64_t>> counter{
      new opentelemetry::metrics::NoopCounter<uint64_t>("test", "none", "unitless")};

  std::map<std::string, std::string> labels = {{"k1", "v1"}};
  counter->Add(10, labels);
  counter->Add(10, labels, opentelemetry::context::Context{});
  counter->Add(2);
  counter->Add(2, opentelemetry::context::Context{});
  counter->Add(10, {{"k1", "1"}, {"k2", 2}});
  counter->Add(10, {{"k1", "1"}, {"k2", 2}}, opentelemetry::context::Context{});
}

TEST(histogram, Record)
{
  std::shared_ptr<opentelemetry::metrics::Histogram<uint64_t>> histogram{
      new opentelemetry::metrics::NoopHistogram<uint64_t>("test", "none", "unitless")};

  std::map<std::string, std::string> labels = {{"k1", "v1"}};
  histogram->Record(10, labels, opentelemetry::context::Context{});
  histogram->Record(2, opentelemetry::context::Context{});

  histogram->Record(10, {{"k1", "1"}, {"k2", 2}}, opentelemetry::context::Context{});
}

TEST(UpDownCountr, Record)
{
  std::shared_ptr<opentelemetry::metrics::UpDownCounter<int64_t>> counter{
      new opentelemetry::metrics::NoopUpDownCounter<int64_t>("test", "none", "unitless")};

  std::map<std::string, std::string> labels = {{"k1", "v1"}};
  counter->Add(10, labels);
  counter->Add(10, labels, opentelemetry::context::Context{});
  counter->Add(2);
  counter->Add(2, opentelemetry::context::Context{});
  counter->Add(10, {{"k1", "1"}, {"k2", 2}});
  counter->Add(10, {{"k1", "1"}, {"k2", 2}}, opentelemetry::context::Context{});
}
