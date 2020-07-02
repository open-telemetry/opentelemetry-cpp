#include "opentelemetry/metrics/noop.h"

#include <map>
#include <memory>

#include <gtest/gtest.h>

using opentelemetry::metrics::NoopMeter;
using opentelemetry::metrics::Meter;

TEST(NoopTest, UseNoopMeters)
{
  std::unique_ptr<Meter> m{new NoopMeter{}};

  m->NewIntCounter("Test int counter", "For testing", "Unitless", true);

  using MAP = std::map<std::string, int>;
  MAP batchValues1 = {{}};
  opentelemetry::trace::KeyValueIterableView<MAP> view{batchValues1};
  m->RecordBatch("abc", view);

  using VEC = std::vector<std::pair<std::string, int>>;
  VEC batchValues2 = {};
  opentelemetry::trace::KeyValueIterableView<VEC> view2{batchValues2};
  m->RecordBatch("abc", view2);

}
