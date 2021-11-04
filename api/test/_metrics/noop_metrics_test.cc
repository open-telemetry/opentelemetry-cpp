// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include <gtest/gtest.h>
#  include "opentelemetry/_metrics/instrument.h"
#  include "opentelemetry/_metrics/noop.h"
#  include "opentelemetry/_metrics/observer_result.h"
#  include "opentelemetry/_metrics/sync_instruments.h"

#  include <array>
#  include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE

using opentelemetry::metrics::Meter;
using opentelemetry::metrics::NoopMeter;

void Callback(opentelemetry::metrics::ObserverResult<int> result)
{
  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};
  result.observe(1, labelkv);
}

TEST(NoopTest, CreateInstruments)
{
  auto m = std::unique_ptr<Meter>(new NoopMeter{});

  // Test instrument constructors
  m->NewIntCounter("Test counter", "For testing", "Unitless", true);
  m->NewIntUpDownCounter("Test ud counter", "For testing", "Unitless", true);
  m->NewIntValueRecorder("Test recorder", "For testing", "Unitless", true);

  m->NewIntSumObserver("Test sum obs", "For testing", "Unitless", true, &Callback);
  m->NewIntUpDownSumObserver("Test udsum obs", "For testing", "Unitless", true, &Callback);
  m->NewIntValueObserver("Test val obs", "For testing", "Unitless", true, &Callback);
}

TEST(NoopMeter, RecordBatch)
{
  // Test BatchRecord with all supported types
  // Create Counter and call RecordBatch for all four supported types: short, int, float, and double

  std::unique_ptr<Meter> m{std::unique_ptr<Meter>(new NoopMeter{})};

  std::map<std::string, std::string> labels = {{"Key", "Value"}};
  auto labelkv = opentelemetry::common::KeyValueIterableView<decltype(labels)>{labels};

  auto s = m->NewShortCounter("Test short counter", "For testing", "Unitless", true);

  std::array<metrics::SynchronousInstrument<short> *, 1> siarr{s.get()};
  std::array<short, 1> svarr{1};
  nostd::span<metrics::SynchronousInstrument<short> *> ssp{siarr};
  nostd::span<short> sval{svarr};
  m->RecordShortBatch(labelkv, ssp, sval);

  auto i = m->NewIntCounter("Test int counter", "For testing", "Unitless", true);

  std::array<metrics::SynchronousInstrument<int> *, 1> iiarr{i.get()};
  std::array<int, 1> ivarr{1};
  nostd::span<metrics::SynchronousInstrument<int> *> isp{iiarr};
  nostd::span<int> ival{ivarr};
  m->RecordIntBatch(labelkv, isp, ival);

  auto f = m->NewFloatCounter("Test int counter", "For testing", "Unitless", true);

  std::array<metrics::SynchronousInstrument<float> *, 1> fiarr{f.get()};
  std::array<float, 1> fvarr{1.0f};
  nostd::span<metrics::SynchronousInstrument<float> *> fsp{fiarr};
  nostd::span<float> fval{fvarr};
  m->RecordFloatBatch(labelkv, fsp, fval);

  auto d = m->NewDoubleCounter("Test int counter", "For testing", "Unitless", true);

  std::array<metrics::SynchronousInstrument<double> *, 1> diarr{d.get()};
  std::array<double, 1> dvarr{1.0f};
  nostd::span<metrics::SynchronousInstrument<double> *> dsp{diarr};
  nostd::span<double> dval{dvarr};
  m->RecordDoubleBatch(labelkv, dsp, dval);
}
OPENTELEMETRY_END_NAMESPACE
#endif
