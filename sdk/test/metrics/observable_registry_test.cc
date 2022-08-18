// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifndef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/sdk/metrics/state/observable_registry.h"
#  include "opentelemetry/metrics/observer_result.h"

#  include <gtest/gtest.h>

using namespace opentelemetry::sdk::metrics;

#  if 0
class MeasurementFetcher
{
public:
  static void Fetcher1(opentelemetry::metrics::ObserverResult &observer_result,
                      void * /*state*/)
  {
    fetch_count1++;
    if (fetch_count1 == 1)
    {
      std::get<observer_result.Observe(20l, {{"RequestType", "GET"}});
      observer_result.Observe(10l, {{"RequestType", "PUT"}});
      number_of_get1 += 20l;
      number_of_put1 += 10l;
    }
    else if (fetch_count1 == 2)
    {
      observer_result.Observe(40l, {{"RequestType", "GET"}});
      observer_result.Observe(20l, {{"RequestType", "PUT"}});
      number_of_get1 += 40l;
      number_of_put1 += 20l;
    }
  }

  static void Fetcher2(opentelemetry::metrics::ObserverResult &observer_result,
                      void * /*state*/)
  {
    fetch_count2++;
    if (fetch_count2 == 1)
    {
      observer_result.Observe(20l, {{"RequestType", "GET"}});
      observer_result.Observe(10l, {{"RequestType", "PUT"}});
      number_of_get2 += 20l;
      number_of_put2 += 10l;
    }
    else if (fetch_count2 == 2)
    {
      observer_result.Observe(40l, {{"RequestType", "GET"}});
      observer_result.Observe(20l, {{"RequestType", "PUT"}});
      number_of_get2 += 40l;
      number_of_put2 += 20l;
    }
  }

  static void init_values()
  {
    fetch_count1 = fetch_count2   = 0;
    number_of_get1 = number_of_get2 = 0;
    number_of_put1 = number_of_put2 = 0;

  }

  static size_t fetch_count1, fetch_count2;
  static long number_of_get1, number_of_get2;
  static long number_of_put1, number_of_put2;
  static const size_t number_of_attributes = 2;  // GET , PUT
};

#  endif

TEST(ObservableRegistry, BasicTests)
{
  ObservableRegistry registry;
  EXPECT_EQ(1, 1);
}
#endif