#include "opentelemetry/metrics/noop.h"

#include <map>
#include <memory>
#include <string>

#include <gtest/gtest.h>

using opentelemetry::meter::NoopMeter;
using opentelemetry::meter::Meter;

TEST(NoopTest, UseNoopMeters)
{
  std::unique_ptr<Meter> m{new NoopMeter{}};

  auto intCounter = m->NewIntCounter("Test int counter", "For testing", "Unitless", true);

  std::map<opentelemetry::metrics::InstrumentKind, double> batchValues1;
  m->RecordBatch("", batchValues1);

  std::vector<std::pair<std::string, double>> batchValues2;
  m->RecordBatch("", batchValues2);

  m->RecordBatch("", {{"IntCounter", 1}, {opentelemetry::metrics::InstrumentKind::Counter, 1.0}});
}
