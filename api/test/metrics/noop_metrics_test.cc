#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/metrics/observer_result.h"

#include <array>
#include <memory>

#include <gtest/gtest.h>

OPENTELEMETRY_BEGIN_NAMESPACE

using opentelemetry::metrics::Meter;
using opentelemetry::metrics::NoopMeter;

void IntCallback(opentelemetry::metrics::IntObserverResult result)
{
  result.observe(1, "key: value");
}

void DoubleCallback(opentelemetry::metrics::DoubleObserverResult result)
{
  result.observe(1.0, "key: value");
}

TEST(NoopTest, UseNoopMeters)
{
  std::unique_ptr<Meter> m{new NoopMeter{}};

  auto t1 = m->NewIntCounter("Test int counter", "For testing", "Unitless", true);
  auto t2 = m->NewIntCounter("Test int counter", "For testing", "Unitless", true);
  auto t3 = m->NewDoubleCounter("Test int counter", "For testing", "Unitless", true);
  m->NewIntUpDownCounter("Test int counter", "For testing", "Unitless", true);
  m->NewDoubleUpDownCounter("Test int counter", "For testing", "Unitless", true);
  m->NewIntValueRecorder("Test int counter", "For testing", "Unitless", true);
  m->NewDoubleValueRecorder("Test int counter", "For testing", "Unitless", true);

  auto alpha =
      m->NewIntSumObserver("Test int counter", "For testing", "Unitless", true, &IntCallback);
  auto beta =
      m->NewDoubleSumObserver("Test int counter", "For testing", "Unitless", true, &DoubleCallback);
  auto gamma =
      m->NewIntUpDownSumObserver("Test int counter", "For testing", "Unitless", true, &IntCallback);
  auto delta = m->NewDoubleUpDownSumObserver("Test int counter", "For testing", "Unitless", true,
                                             &DoubleCallback);
  auto epsilon =
      m->NewIntValueObserver("Test int counter", "For testing", "Unitless", true, &IntCallback);
  auto zeta = m->NewDoubleValueObserver("Test int counter", "For testing", "Unitless", true,
                                        &DoubleCallback);

  alpha->observe(1, "key:value");
  beta->observe(1.0, "key:value");
  gamma->observe(1, "key:value");
  delta->observe(1.0, "key:value");
  epsilon->observe(1, "key:value");
  zeta->observe(1.0, "key:value");

  ASSERT_NE(t1, t2);

  using M =
  std::pair<nostd::shared_ptr<metrics::SynchronousInstrument>, nostd::variant<int, double>>;
  M t1_pair;
  nostd::shared_ptr<metrics::SynchronousInstrument> tmp = t1;
  t1_pair.first                                         = tmp;
  t1_pair.second                                        = 1;

  M t3_pair;
  nostd::shared_ptr<metrics::SynchronousInstrument> tmp2 = t3;
  t3_pair.first                                          = tmp2;
  t3_pair.second                                         = 2.0;

  std::array<M, 2> arr{t1_pair, t3_pair};
  nostd::span<M> span(arr);

  m->RecordBatch("key: value", span);
}
OPENTELEMETRY_END_NAMESPACE
