// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "opentelemetry/context/context.h"
#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/metrics/sync_instruments.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
#  include "opentelemetry/common/macros.h"
#endif

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

#if OPENTELEMETRY_ABI_VERSION_NO >= 2
TEST(SynchronousInstrument, NoopInstrumentsAreDisabled)
{
  opentelemetry::metrics::NoopCounter<uint64_t> counter("test", "none", "unitless");
  opentelemetry::metrics::NoopHistogram<double> histogram("test", "none", "unitless");
  opentelemetry::metrics::NoopUpDownCounter<int64_t> up_down_counter("test", "none", "unitless");
  opentelemetry::metrics::NoopGauge<double> gauge("test", "none", "unitless");

  EXPECT_FALSE(counter.Enabled());
  EXPECT_FALSE(histogram.Enabled());
  EXPECT_FALSE(up_down_counter.Enabled());
  EXPECT_FALSE(gauge.Enabled());
}

namespace
{
// An instrument reading an external flag, the way an SDK instrument does.
class FlagCounter : public opentelemetry::metrics::NoopCounter<uint64_t>
{
public:
  explicit FlagCounter(const bool *flag) : NoopCounter("test", "none", "unitless")
  {
    SetEnabledFlag(flag);
  }
};
}  // namespace

// A copy must not read the default flag stored in the instrument it was copied from.
TEST(SynchronousInstrument, CopyOfDefaultInstrumentIsDisabled)
{
  std::unique_ptr<opentelemetry::metrics::NoopCounter<uint64_t>> original{
      new opentelemetry::metrics::NoopCounter<uint64_t>("test", "none", "unitless")};
  opentelemetry::metrics::NoopCounter<uint64_t> copy(*original);
  original.reset();
  EXPECT_FALSE(copy.Enabled());
}

// A copy keeps reading the external flag after the instrument it was copied from is destroyed.
TEST(SynchronousInstrument, CopySharesExternalFlag)
{
  bool flag = true;
  std::unique_ptr<FlagCounter> original{new FlagCounter(&flag)};
  FlagCounter copy(*original);
  original.reset();
  EXPECT_TRUE(copy.Enabled());

  OPENTELEMETRY_ATOMIC_WRITE_8(&flag, false);
  EXPECT_FALSE(copy.Enabled());
}
#endif
