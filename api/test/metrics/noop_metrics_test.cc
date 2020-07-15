#include <gtest/gtest.h>
#include "opentelemetry/metrics/noop.h"
#include "opentelemetry/metrics/observer_result.h"

#include <array>
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
  std::unique_ptr<Meter<int>> m{std::unique_ptr<Meter<int>>(new NoopMeter<int>{})};

  // Test instrument constructors
  auto t1 = m->NewCounter("Test counter", "For testing", "Unitless", true);
  auto t2 = m->NewCounter("Test counter", "For testing", "Unitless", true);
  m->NewUpDownCounter("Test ud counter", "For testing", "Unitless", true);
  m->NewValueRecorder("Test recorder", "For testing", "Unitless", true);

  auto alpha =
      m->NewSumObserver("Test sum obs", "For testing", "Unitless", true,
                           &Callback);
  auto beta =
      m->NewUpDownSumObserver("Test udsum obs", "For testing", "Unitless", true,
                                 &Callback);
  auto gamma =
      m->NewValueObserver("Test val obs", "For testing", "Unitless", true,
                             &Callback);

  // Two instruments with the same characteristic should NOT be equal.
  // Note: In actual (non-noop) implementations of the Meter class, creating two
  // instruments with the same name is disallowed.
  ASSERT_NE(t1, t2);
}

TEST(NoopMeter, RecordBatch)
{
  // Test BatchRecord
  // This is awkward to use because BatchRecord takes a KeyValueIterable and nostd::span of
  // std::pairs as arguments.
  // This is to adhere to ABI stability.
  std::unique_ptr<Meter<int>> m{std::unique_ptr<Meter<int>>(new NoopMeter<int>{})};
  auto t1 = m->NewCounter("Test counter", "For testing", "Unitless", true);
  auto t2 = m->NewCounter("Test counter", "For testing", "Unitless", true);

  using M =
  std::pair<nostd::shared_ptr<metrics::SynchronousInstrument<int>>, int>;

  // (t1, 1)
  M t1_pair;
  t1_pair.first = t1;
  t1_pair.second = 1;

  // (t2, 2)
  M t2_pair;
  t2_pair.first = t2;
  t2_pair.second = 2;

  // Create a std::array and use that array to create a span.
  std::array<M, 2> arr{t1_pair, t2_pair};
  nostd::span<M> span(arr);

  // Create a KeyValueIterableView for labels
  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = trace::KeyValueIterableView<decltype(labels)>{labels};

  // Test all versions of RecordBatch
  m->RecordBatch(labelkv, span);
  m->RecordBatch({{"Key", "Value"}}, span);
  m->RecordBatch({{"Key", "Value"}}, {{t1, 1}, {t2, 2}});
}

TEST(NoopMeter, InstrumentTypes)
{
  std::unique_ptr<Meter<int>> m{std::unique_ptr<Meter<int>>(new NoopMeter<int>{})};

  auto counter =
      m->NewCounter("Test counter", "For testing", "Unitless", true);

  auto other_counter =
      m->NewCounter("Other test counter", "For testing", "Unitless", true);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = trace::KeyValueIterableView<decltype(labels)>{labels};

  counter->update(1, labelkv);
  other_counter->update(1.0, labelkv);
}
OPENTELEMETRY_END_NAMESPACE
