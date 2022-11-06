// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <future>
#include <map>
#include <thread>

#include "opentelemetry/exporters/prometheus/collector.h"
#include "opentelemetry/version.h"
#include "prometheus_test_helper.h"

using opentelemetry::exporter::metrics::PrometheusCollector;
namespace metric_api = opentelemetry::metrics;
namespace metric_sdk = opentelemetry::sdk::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE

// ==================== Test for addMetricsData() function ======================

/**
 * AddMetricData() should be able to successfully add a collection
 * of SumPointData. It checks that the cumulative
 * sum of updates to the aggregator of a record before and after AddMetricData()
 * is called are equal.
 */
TEST(PrometheusCollector, AddMetricDataWithCounterRecordsSuccessfully)
{
  PrometheusCollector collector;

  // construct a collection of records with CounterAggregators and double
  auto data = CreateSumPointData();

  // add records to collection
  collector.AddMetricData(data);

  // Collection size should be the same as the size
  // of the records collection passed to addMetricData()
  ASSERT_EQ(collector.GetCollection().size(), 1);

  // check values of records created vs records from metricsToCollect,
  // accessed by getCollection()

  auto &&collector_data = collector.GetCollection();
  ASSERT_EQ(collector_data.at(0)->resource_, data.resource_);
  for (auto &collector_d : collector_data)
  {
    for (uint32_t instrument_idx = 0; instrument_idx < collector_d->scope_metric_data_.size();
         ++instrument_idx)
    {
      ASSERT_EQ(collector_d->scope_metric_data_.at(instrument_idx).scope_,
                data.scope_metric_data_.at(instrument_idx).scope_);
      ASSERT_EQ(collector_d->scope_metric_data_.at(instrument_idx).metric_data_,
                data.scope_metric_data_.at(instrument_idx).metric_data_);
    }
  }
}

/**
 * AddMetricData() should be able to successfully add a collection
 * of HistogramPointData. It checks that the cumulative
 * sum of updates to the aggregator of a record before and after AddMetricData()
 * is called are equal.
 */
TEST(PrometheusCollector, AddMetricDataWithHistogramSuccessfully)
{
  PrometheusCollector collector;

  // construct a collection of records with CounterAggregators and double
  auto data = CreateHistogramPointData();

  // add records to collection
  collector.AddMetricData(data);

  // Collection size should be the same as the size
  // of the records collection passed to addMetricData()
  ASSERT_EQ(collector.GetCollection().size(), 1);

  // check values of records created vs records from metricsToCollect,
  // accessed by getCollection()

  auto &&collector_data = collector.GetCollection();
  ASSERT_EQ(collector_data.at(0)->resource_, data.resource_);
  for (auto &collector_d : collector_data)
  {
    for (uint32_t instrument_idx = 0; instrument_idx < collector_d->scope_metric_data_.size();
         ++instrument_idx)
    {
      ASSERT_EQ(collector_d->scope_metric_data_.at(instrument_idx).scope_,
                data.scope_metric_data_.at(instrument_idx).scope_);
      ASSERT_EQ(collector_d->scope_metric_data_.at(instrument_idx).metric_data_,
                data.scope_metric_data_.at(instrument_idx).metric_data_);
    }
  }
}

/**
 * AddMetricData() should be able to successfully add a collection
 * of LastValuePointData. It checks that the cumulative
 * sum of updates to the aggregator of a record before and after AddMetricData()
 * is called are equal.
 */
TEST(PrometheusCollector, AddMetricDataWithLastValueSuccessfully)
{
  PrometheusCollector collector;

  // construct a collection of records with CounterAggregators and double
  auto data = CreateLastValuePointData();

  // add records to collection
  collector.AddMetricData(data);

  // Collection size should be the same as the size
  // of the records collection passed to addMetricData()
  ASSERT_EQ(collector.GetCollection().size(), 1);

  // check values of records created vs records from metricsToCollect,
  // accessed by getCollection()

  auto &&collector_data = collector.GetCollection();
  ASSERT_EQ(collector_data.at(0)->resource_, data.resource_);
  for (auto &collector_d : collector_data)
  {
    for (uint32_t instrument_idx = 0; instrument_idx < collector_d->scope_metric_data_.size();
         ++instrument_idx)
    {
      ASSERT_EQ(collector_d->scope_metric_data_.at(instrument_idx).scope_,
                data.scope_metric_data_.at(instrument_idx).scope_);
      ASSERT_EQ(collector_d->scope_metric_data_.at(instrument_idx).metric_data_,
                data.scope_metric_data_.at(instrument_idx).metric_data_);
    }
  }
}

/**
 * AddMetricData() should be able to successfully add a collection
 * of DropPointData. It checks that the cumulative
 * sum of updates to the aggregator of a record before and after AddMetricData()
 * is called are equal.
 */
TEST(PrometheusCollector, AddMetricDataWithDropSuccessfully)
{
  PrometheusCollector collector;

  // construct a collection of records with CounterAggregators and double
  auto data = CreateDropPointData();

  // add records to collection
  collector.AddMetricData(data);

  // Collection size should be the same as the size
  // of the records collection passed to addMetricData()
  ASSERT_EQ(collector.GetCollection().size(), 1);

  // check values of records created vs records from metricsToCollect,
  // accessed by getCollection()

  auto &&collector_data = collector.GetCollection();
  ASSERT_EQ(collector_data.at(0)->resource_, data.resource_);
  for (auto &collector_d : collector_data)
  {
    for (uint32_t instrument_idx = 0; instrument_idx < collector_d->scope_metric_data_.size();
         ++instrument_idx)
    {
      ASSERT_EQ(collector_d->scope_metric_data_.at(instrument_idx).scope_,
                data.scope_metric_data_.at(instrument_idx).scope_);
      ASSERT_EQ(collector_d->scope_metric_data_.at(instrument_idx).metric_data_,
                data.scope_metric_data_.at(instrument_idx).metric_data_);
    }
  }
}

TEST(PrometheusCollector, AddMetricDataDoesNotAddWithInsufficentSpace)
{
  PrometheusCollector collector;

  int max_collection_size = collector.GetMaxCollectionSize();

  for (int count = 1; count <= max_collection_size; ++count)
  {
    collector.AddMetricData(CreateSumPointData());
    ASSERT_EQ(count, collector.GetCollection().size());
  }

  // Try adding one more collection of records again to
  // metricsToCollect.
  collector.AddMetricData(CreateSumPointData());

  // Check that the number of records in metricsToCollect
  // has not changed.
  ASSERT_EQ(collector.GetCollection().size(), max_collection_size);
}

// ==================== Test for Constructor ======================
TEST(PrometheusCollector, ConstructorInitializesCollector)
{
  PrometheusCollector collector;

  // current size should be 0, capacity should be set to default
  ASSERT_EQ(collector.GetCollection().size(), 0);
}

// ==================== Tests for collect() function ======================

/**
 * When collector is initialized, the collection inside is should also be initialized
 */
TEST(PrometheusCollector, CollectInitializesMetricFamilyCollection)
{
  PrometheusCollector collector;
  auto c1 = collector.Collect();
  ASSERT_EQ(c1.size(), 0);
}

/**
 * Collect function should collect all data and clear the intermediate collection
 */
TEST(PrometheusCollector, CollectClearsTheCollection)
{
  PrometheusCollector collector;

  // construct a collection to add metric records
  collector.AddMetricData(CreateSumPointData());
  collector.AddMetricData(CreateSumPointData());

  // the collection should not be empty now
  ASSERT_EQ(collector.GetCollection().size(), 2);

  // don't care the collected result in this test
  collector.Collect();

  // after the collect() call, the collection should be empty
  ASSERT_EQ(collector.GetCollection().size(), 0);
}

/**
 * Collected data should be already be parsed to Prometheus Metric format
 */
TEST(PrometheusCollector, CollectParsesDataToMetricFamily)
{
  PrometheusCollector collector;

  collector.AddMetricData(CreateSumPointData());

  // the collection should not be empty now
  ASSERT_EQ(collector.GetCollection().size(), 1);
  auto collected = collector.Collect();

  ASSERT_EQ(collected.size(), 1);

  auto metric_family = collected[0];

  // Collect function really collects a vector of MetricFamily
  ASSERT_EQ(metric_family.name, "library_name");
  ASSERT_EQ(metric_family.help, "description");
  ASSERT_EQ(metric_family.type, prometheus_client::MetricType::Counter);
  ASSERT_EQ(metric_family.metric.size(), 2);
  ASSERT_DOUBLE_EQ(metric_family.metric[0].counter.value, 10);
}

/**
 * Concurrency Test 1: After adding data concurrently, the intermediate collection should
 * contain all data from all threads.
 */
TEST(PrometheusCollector, ConcurrencyAddingRecords)
{
  PrometheusCollector collector;

  std::thread first(&PrometheusCollector::AddMetricData, std::ref(collector), CreateSumPointData());
  std::thread second(&PrometheusCollector::AddMetricData, std::ref(collector),
                     CreateSumPointData());

  first.join();
  second.join();

  ASSERT_EQ(collector.GetCollection().size(), 2);
}

/**
 * Concurrency Test 2: After adding data concurrently and collecting, the intermediate collection
 * should be empty, and all data are collected in the result vector.
 */
TEST(PrometheusCollector, ConcurrentlyAddingAndThenCollecting)
{
  PrometheusCollector collector;

  std::thread first(&PrometheusCollector::AddMetricData, std::ref(collector), CreateSumPointData());
  std::thread second(&PrometheusCollector::AddMetricData, std::ref(collector),
                     CreateSumPointData());
  first.join();
  second.join();

  auto collect_future = std::async(&PrometheusCollector::Collect, std::ref(collector));
  auto res            = collect_future.get();

  ASSERT_EQ(collector.GetCollection().size(), 0);
  ASSERT_EQ(res.size(), 2);
}

/**
 * Concurrency Test 3: Concurrently adding and collecting. We don't know when the collect function
 * is called, but all data entries are either collected or left in the collection.
 */
TEST(PrometheusCollector, ConcurrentlyAddingAndCollecting)
{
  PrometheusCollector collector;

  // construct a collection to add metric records

  std::thread first(&PrometheusCollector::AddMetricData, std::ref(collector), CreateSumPointData());
  std::thread second(&PrometheusCollector::AddMetricData, std::ref(collector),
                     CreateSumPointData());
  auto collect_future = std::async(&PrometheusCollector::Collect, std::ref(collector));

  first.join();
  second.join();

  auto res = collect_future.get();

  // the size of collection can be 0, 2, 4, because we don't know when the collect()
  // is really called. However, we claim that if the data in the collection is collected,
  // they must be in the res. So res.size() + collection.size() must be the total number
  // of data records we generated.
  ASSERT_EQ(res.size() + collector.GetCollection().size(), 2);
}

/**
 * Concurrency Test 4: Concurrently adding then concurrently collecting. We don't know which
 * collecting thread fetches all data, but either one should succeed.
 */
TEST(PrometheusCollector, ConcurrentlyAddingAndConcurrentlyCollecting)
{
  PrometheusCollector collector;

  // concurrently adding
  std::thread first(&PrometheusCollector::AddMetricData, std::ref(collector), CreateSumPointData());
  std::thread second(&PrometheusCollector::AddMetricData, std::ref(collector),
                     CreateSumPointData());
  first.join();
  second.join();

  // after adding, then concurrently consuming
  auto collect_future1 = std::async(&PrometheusCollector::Collect, std::ref(collector));
  auto collect_future2 = std::async(&PrometheusCollector::Collect, std::ref(collector));
  auto res1            = collect_future1.get();
  auto res2            = collect_future2.get();

  // all added data must be collected in either res1 or res2
  ASSERT_EQ(res1.size() + res2.size(), 2);
}

OPENTELEMETRY_END_NAMESPACE
