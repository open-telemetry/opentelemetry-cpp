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

  // Test instrument constructors
  auto t1 = m->NewIntCounter("Test int counter", "For testing", "Unitless", true);
  auto t2 = m->NewIntCounter("Test int counter", "For testing", "Unitless", true);
  auto t3 = m->NewDoubleCounter("Test double counter", "For testing", "Unitless", true);
  m->NewIntUpDownCounter("Test intud counter", "For testing", "Unitless", true);
  m->NewDoubleUpDownCounter("Test doubleud counter", "For testing", "Unitless", true);
  m->NewIntValueRecorder("Test int recorder", "For testing", "Unitless", true);
  m->NewDoubleValueRecorder("Test double recorder", "For testing", "Unitless", true);

  auto alpha =
      m->NewIntSumObserver("Test int sum obs", "For testing", "Unitless", true,
                           &IntCallback);
  auto beta =
      m->NewDoubleSumObserver("Test double sum obs", "For testing", "Unitless", true,
                              &DoubleCallback);
  auto gamma =
      m->NewIntUpDownSumObserver("Test intud sum obs", "For testing", "Unitless", true,
                                 &IntCallback);
  auto delta =
      m->NewDoubleUpDownSumObserver("Test doubleud sum obs", "For testing", "Unitless", true,
                                    &DoubleCallback);
  auto epsilon =
      m->NewIntValueObserver("Test int val obs", "For testing", "Unitless", true,
                             &IntCallback);
  auto zeta =
      m->NewDoubleValueObserver("Test double val obs", "For testing", "Unitless", true,
                                &DoubleCallback);

  // Two instruments with the same characteristic should NOT be equal.
  // Note: In actual (non-noop) implementations of the Meter class, creating two
  // instruments with the same name is disallowed.
  ASSERT_NE(t1, t2);

  // Test BatchRecord
  // This is awkward to use because BatchRecord takes a KeyValueIterable and nostd::span of
  // std::pairs as arguments.
  // This is to adhere to ABI stability.
  using M =
  std::pair<nostd::shared_ptr<metrics::SynchronousInstrument>, nostd::variant<int, double>>;

  // (t1, 1)
  M t1_pair;
  t1_pair.first = t1;
  t1_pair.second = 1;

  // (t3, 2.0)
  M t3_pair;
  t3_pair.first = t3;
  t3_pair.second = 2.0;

  // Create a std::array and use that array to create a span.
  std::array<M, 2> arr{t1_pair, t3_pair};
  nostd::span<M> span(arr);

  using N = std::map<std::string, common::AttributeValue>;

  N mp = {{"key", "value"}};

  // Test all versions of RecordBatch
  m->RecordBatch(mp, span);
  m->RecordBatch({{"Key", "Value"}}, span);
  m->RecordBatch({{"Key", "Value"}}, {{t1, 1}, {t3, 2.0}});
}
OPENTELEMETRY_END_NAMESPACE
