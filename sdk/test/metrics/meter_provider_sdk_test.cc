#include "opentelemetry/sdk/metrics/meter.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"

#include <gtest/gtest.h>

using namespace opentelemetry::sdk::metrics;

TEST(MeterProvider, GetMeter)
{
  MeterProvider tf;
  auto t1 = tf.GetMeter("test");
  auto t2 = tf.GetMeter("test");
  auto t3 = tf.GetMeter("different", "1.0.0");
  ASSERT_NE(nullptr, t1);
  ASSERT_NE(nullptr, t2);
  ASSERT_NE(nullptr, t3);

  // Should return the same instance each time.
  ASSERT_EQ(t1, t2);
  ASSERT_EQ(t1, t3);
}
