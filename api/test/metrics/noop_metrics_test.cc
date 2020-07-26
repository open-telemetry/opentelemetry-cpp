#include <gtest/gtest.h>
#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/metrics/observer_result.h"
#include "opentelemetry/metrics/sync_instruments.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE

using opentelemetry::metrics::Meter;
using opentelemetry::metrics::NoopMeter;

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

  m->NewIntSumObserver("Test sum obs", "For testing", "Unitless", true,
                           &Callback);
  m->NewIntUpDownSumObserver("Test udsum obs", "For testing", "Unitless", true,
                                 &Callback);
  m->NewIntValueObserver("Test val obs", "For testing", "Unitless", true,
                             &Callback);
}

TEST(NoopMeter, RecordBatch)
{
  // Test BatchRecord with all supported types
  // Create Counter and call RecordBatch for all four supported types: short, int, float, and double

  std::unique_ptr<Meter> m{std::unique_ptr<Meter>(new NoopMeter{})};

  std::map<std::string, std::string> labels = {{"Key", "Value"}};
  auto labelkv = opentelemetry::trace::KeyValueIterableView<decltype(labels)>{labels};

  auto s = m->NewShortCounter("Test short counter", "For testing", "Unitless", true);

  nostd::shared_ptr<opentelemetry::metrics::SynchronousInstrument<short>>
      sinstr_arr[] =
          {nostd::shared_ptr<opentelemetry::metrics::SynchronousInstrument<short>>(s.get())};
  short svalues_arr[] = {1};

  nostd::span<const nostd::shared_ptr<opentelemetry::metrics::SynchronousInstrument<short>>> sinstrs
      {sinstr_arr};
  nostd::span<const short, 1> svalues {svalues_arr};

  m->RecordShortBatch(labelkv, sinstrs, svalues);

  auto i = m->NewIntCounter("Test int counter", "For testing", "Unitless", true);

  nostd::shared_ptr<opentelemetry::metrics::SynchronousInstrument<int>>
      iinstr_arr[] =
          {nostd::shared_ptr<opentelemetry::metrics::SynchronousInstrument<int>>(i.get())};
  int ivalues_arr[] = {1};

  nostd::span<const nostd::shared_ptr<opentelemetry::metrics::SynchronousInstrument<int>>> iinstrs
      {iinstr_arr};
  nostd::span<const int, 1> ivalues {ivalues_arr};
  m->RecordIntBatch(labelkv, iinstrs, ivalues);

  auto f = m->NewFloatCounter("Test int counter", "For testing", "Unitless", true);

  nostd::shared_ptr<opentelemetry::metrics::SynchronousInstrument<float>>
      finstr_arr[] =
          {nostd::shared_ptr<opentelemetry::metrics::SynchronousInstrument<float>>(f.get())};
  float fvalues_arr[] = {1.0};

  nostd::span<const nostd::shared_ptr<opentelemetry::metrics::SynchronousInstrument<float>>>
      finstrs {finstr_arr};
  nostd::span<const float, 1> fvalues {fvalues_arr};
  m->RecordFloatBatch(labelkv, finstrs, fvalues);

  auto d = m->NewDoubleCounter("Test int counter", "For testing", "Unitless", true);

  nostd::shared_ptr<opentelemetry::metrics::SynchronousInstrument<double>>
      dinstr_arr[] =
          {nostd::shared_ptr<opentelemetry::metrics::SynchronousInstrument<double>>(d.get())};
  double dvalues_arr[] = {1.0};

  nostd::span<const nostd::shared_ptr<opentelemetry::metrics::SynchronousInstrument<double>>>
      dinstrs {dinstr_arr};
  nostd::span<const double, 1> dvalues {dvalues_arr};
  m->RecordDoubleBatch(labelkv, dinstrs, dvalues);

}
OPENTELEMETRY_END_NAMESPACE
