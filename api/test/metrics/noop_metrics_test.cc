#include <gtest/gtest.h>
#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/metrics/observer_result.h"
#include "opentelemetry/metrics/sync_instruments.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE

using opentelemetry::metrics::Meter;
using opentelemetry::metrics::NoopMeter;

namespace metrics_api = opentelemetry::metrics;

void Callback(opentelemetry::metrics::ObserverResult<int> result)
{
  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv                              = trace::KeyValueIterableView<decltype(labels)>{labels};
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
  auto labelkv = opentelemetry::trace::KeyValueIterableView<decltype(labels)>{labels};

  auto s = m->NewShortCounter("Test short counter", "For testing", "Unitless", true);

  m->RecordShortBatch({{"Key", "Value"}},
                      {nostd::dynamic_pointer_cast<metrics_api::SynchronousInstrument<short>>(s)},
                      {1});

  auto i = m->NewIntCounter("Test int counter", "For testing", "Unitless", true);

  m->RecordIntBatch({{"Key", "Value"}},
                    {nostd::dynamic_pointer_cast<metrics_api::SynchronousInstrument<int>>(i)}, {1});

  auto f = m->NewFloatCounter("Test int counter", "For testing", "Unitless", true);

  m->RecordFloatBatch({{"Key", "Value"}},
                      {nostd::dynamic_pointer_cast<metrics_api::SynchronousInstrument<float>>(f)},
                      {1.0});

  auto d = m->NewDoubleCounter("Test int counter", "For testing", "Unitless", true);

  m->RecordDoubleBatch({{"Key", "Value"}},
                       {nostd::dynamic_pointer_cast<metrics_api::SynchronousInstrument<double>>(d)},
                       {1});
}
OPENTELEMETRY_END_NAMESPACE
