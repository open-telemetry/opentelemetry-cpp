// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include <gtest/gtest.h>
#  include <future>
#  include <map>
#  include <thread>

#  include "opentelemetry/_metrics/instrument.h"
#  include "opentelemetry/exporters/prometheus/prometheus_collector.h"
#  include "opentelemetry/sdk/_metrics/aggregator/aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/counter_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/exact_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/gauge_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/histogram_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/min_max_sum_count_aggregator.h"
#  include "opentelemetry/sdk/_metrics/aggregator/sketch_aggregator.h"
#  include "opentelemetry/sdk/_metrics/record.h"
#  include "opentelemetry/version.h"

using opentelemetry::exporter::prometheus::PrometheusCollector;
namespace metric_api = opentelemetry::metrics;
namespace metric_sdk = opentelemetry::sdk::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE

/**
 * CreateAgg() is a helper function that returns a
 * nostd::shared_ptr given an AggregatorKind
 */
template <typename T>
std::shared_ptr<metric_sdk::Aggregator<T>> CreateAgg(metric_sdk::AggregatorKind kind,
                                                     bool exactMode = true)
{
  std::shared_ptr<metric_sdk::Aggregator<T>> aggregator;
  switch (kind)
  {
    case metric_sdk::AggregatorKind::Counter: {
      aggregator = std::shared_ptr<metric_sdk::Aggregator<T>>(
          new metric_sdk::CounterAggregator<T>(metric_api::InstrumentKind::Counter));
      break;
    }
    case metric_sdk::AggregatorKind::MinMaxSumCount: {
      aggregator = std::shared_ptr<metric_sdk::Aggregator<T>>(
          new metric_sdk::MinMaxSumCountAggregator<T>(metric_api::InstrumentKind::Counter));
      break;
    }
    case metric_sdk::AggregatorKind::Gauge: {
      aggregator = std::shared_ptr<metric_sdk::Aggregator<T>>(
          new metric_sdk::GaugeAggregator<T>(metric_api::InstrumentKind::Counter));
      break;
    }
    case metric_sdk::AggregatorKind::Sketch: {
      aggregator = std::shared_ptr<metric_sdk::Aggregator<T>>(
          new metric_sdk::SketchAggregator<T>(metric_api::InstrumentKind::Counter, 0.000005));
      break;
    }
    case metric_sdk::AggregatorKind::Histogram: {
      std::vector<double> boundaries{10, 20};
      aggregator = std::shared_ptr<metric_sdk::Aggregator<T>>(
          new metric_sdk::HistogramAggregator<T>(metric_api::InstrumentKind::Counter, boundaries));
      break;
    }
    case metric_sdk::AggregatorKind::Exact: {
      aggregator = std::shared_ptr<metric_sdk::Aggregator<T>>(
          new metric_sdk::ExactAggregator<T>(metric_api::InstrumentKind::Counter, exactMode));
      break;
    }
    default:
      aggregator = nullptr;
  }
  return aggregator;
}

/**
 * Populate() updates the aggregator with values and checkpoints it based
 * on what its AggregatorKind is
 */
template <typename T>
void Populate(std::shared_ptr<metric_sdk::Aggregator<T>> &aggregator)
{
  if (aggregator->get_aggregator_kind() == metric_sdk::AggregatorKind::Counter)
  {
    aggregator->update(10.0);
    aggregator->update(5.0);
    aggregator->checkpoint();
  }
  else if (aggregator->get_aggregator_kind() == metric_sdk::AggregatorKind::MinMaxSumCount)
  {
    aggregator->update(10);
    aggregator->update(2);
    aggregator->update(5);
    aggregator->checkpoint();
  }
  else if (aggregator->get_aggregator_kind() == metric_sdk::AggregatorKind::Gauge)
  {
    aggregator->update(10);
    aggregator->update(5);
    aggregator->checkpoint();
  }
  else if (aggregator->get_aggregator_kind() == metric_sdk::AggregatorKind::Sketch)
  {
    for (double i = 0; i < 10.0; i++)
    {
      aggregator->update(i);
    }
    aggregator->checkpoint();
  }
  else if (aggregator->get_aggregator_kind() == metric_sdk::AggregatorKind::Histogram)
  {
    for (float i = 0; i < 30.0; i++)
    {
      aggregator->update(i);
    }
    aggregator->checkpoint();
  }
  else if (aggregator->get_aggregator_kind() == metric_sdk::AggregatorKind::Exact)
  {
    for (double i = 0; i < 10.0; i++)
    {
      aggregator->update(i);
    }
    aggregator->checkpoint();
  }
}

/**
 * Helper function to create a collection of records taken from
 * a aggregator of specified AggregatorKind
 */
template <typename T>
std::vector<metric_sdk::Record> CreateRecords(int num,
                                              metric_sdk::AggregatorKind kind,
                                              bool exactMode = true)
{
  std::vector<metric_sdk::Record> records;

  for (int i = 0; i < num; i++)
  {
    std::string name        = "record-" + std::to_string(i);
    std::string description = "record " + std::to_string(i) + " for test purpose";
    std::string labels      = "{label1:v1,label2:v2,}";
    std::shared_ptr<metric_sdk::Aggregator<T>> aggregator = CreateAgg<T>(kind, exactMode);
    Populate(aggregator);

    metric_sdk::Record r{name, description, labels, aggregator};
    records.push_back(r);
  }
  return records;
}

// ==================== Test for addMetricsData() function ======================

/**
 * AddMetricData() should be able to successfully add a collection
 * of Records with Counter Aggregators. It checks that the cumulative
 * sum of updates to the aggregator of a record before and after AddMetricData()
 * is called are equal.
 */
TEST(PrometheusCollector, AddMetricDataWithCounterRecordsSuccessfully)
{
  PrometheusCollector collector;

  // number of records to create
  int num_records = 2;

  // construct a collection of records with CounterAggregators and double
  std::vector<metric_sdk::Record> records =
      CreateRecords<double>(num_records, metric_sdk::AggregatorKind::Counter);

  // add records to collection
  collector.AddMetricData(records);

  // Collection size should be the same as the size
  // of the records collection passed to addMetricData()
  ASSERT_EQ(collector.GetCollection().size(), records.size());

  // check values of records created vs records from metricsToCollect,
  // accessed by getCollection()

  for (int i = 0; i < num_records; i++)
  {
    metric_sdk::Record before = records[i];
    metric_sdk::Record after  = collector.GetCollection()[i];

    ASSERT_EQ(before.GetName(), after.GetName());

    ASSERT_EQ(before.GetDescription(), after.GetDescription());

    ASSERT_EQ(before.GetLabels(), after.GetLabels());

    auto before_agg_var = before.GetAggregator();
    auto before_agg = nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(before_agg_var);

    auto after_agg_var = after.GetAggregator();
    auto after_agg     = nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(after_agg_var);

    ASSERT_EQ(before_agg->get_checkpoint().size(), after_agg->get_checkpoint().size());
    for (size_t i = 0; i < before_agg->get_checkpoint().size(); i++)
    {
      ASSERT_EQ(before_agg->get_checkpoint()[i], after_agg->get_checkpoint()[i]);
    }
  }
}

/**
 * AddMetricData() should be able to successfully add a collection
 * of Records with MinMaxSumCount Aggregators. It checks that the min, max,
 * sum, and count of updates to the aggregator of a record before and after AddMetricData()
 * is called are equal.
 */
TEST(PrometheusCollector, AddMetricDataWithMinMaxSumCountRecordsSuccessfully)
{
  PrometheusCollector collector;

  // number of records to create
  int num_records = 2;

  // construct a collection of records with MinMaxSumCountAggregators and short
  std::vector<metric_sdk::Record> records =
      CreateRecords<short>(num_records, metric_sdk::AggregatorKind::MinMaxSumCount);

  // add records to collection
  collector.AddMetricData(records);

  // Collection size should be the same as the size
  // of the records collection passed to addMetricData()
  ASSERT_EQ(collector.GetCollection().size(), records.size());

  // check values of records created vs records from metricsToCollect,
  // accessed by getCollection()

  for (int i = 0; i < num_records; i++)
  {
    metric_sdk::Record before = records[i];
    metric_sdk::Record after  = collector.GetCollection()[i];

    ASSERT_EQ(before.GetName(), after.GetName());

    ASSERT_EQ(before.GetDescription(), after.GetDescription());

    ASSERT_EQ(before.GetLabels(), after.GetLabels());

    auto before_agg_var = before.GetAggregator();
    auto before_agg = nostd::get<std::shared_ptr<metric_sdk::Aggregator<short>>>(before_agg_var);

    auto after_agg_var = after.GetAggregator();
    auto after_agg     = nostd::get<std::shared_ptr<metric_sdk::Aggregator<short>>>(after_agg_var);

    ASSERT_EQ(before_agg->get_checkpoint().size(), after_agg->get_checkpoint().size());
    for (size_t i = 0; i < before_agg->get_checkpoint().size(); i++)
    {
      ASSERT_EQ(before_agg->get_checkpoint()[i], after_agg->get_checkpoint()[i]);
    }
  }
}

/**
 * AddMetricData() should be able to successfully add a collection
 * of Records with Gauge Aggregators. It checks that the last update
 * to the aggregator of a record before and after AddMetricData()
 * is called are equal.
 */
TEST(PrometheusCollector, AddMetricDataWithGaugeRecordsSuccessfully)
{
  PrometheusCollector collector;

  // number of records to create
  int num_records = 2;

  // construct a collection of records with GaugeAggregators and int
  std::vector<metric_sdk::Record> records =
      CreateRecords<int>(num_records, metric_sdk::AggregatorKind::Gauge);

  // add records to collection
  collector.AddMetricData(records);

  // Collection size should be the same as the size
  // of the records collection passed to addMetricData()
  ASSERT_EQ(collector.GetCollection().size(), records.size());

  // check values of records created vs records from metricsToCollect,
  // accessed by getCollection()

  for (int i = 0; i < num_records; i++)
  {
    metric_sdk::Record before = records[i];
    metric_sdk::Record after  = collector.GetCollection()[i];

    ASSERT_EQ(before.GetName(), after.GetName());

    ASSERT_EQ(before.GetDescription(), after.GetDescription());

    ASSERT_EQ(before.GetLabels(), after.GetLabels());

    auto before_agg_var = before.GetAggregator();
    auto before_agg     = nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(before_agg_var);

    auto after_agg_var = after.GetAggregator();
    auto after_agg     = nostd::get<std::shared_ptr<metric_sdk::Aggregator<int>>>(after_agg_var);

    ASSERT_EQ(before_agg->get_checkpoint().size(), after_agg->get_checkpoint().size());
    for (size_t i = 0; i < before_agg->get_checkpoint().size(); i++)
    {
      ASSERT_EQ(before_agg->get_checkpoint()[i], after_agg->get_checkpoint()[i]);
    }
  }
}

/**
 * AddMetricData() should be able to successfully add a collection
 * of Records with Sketch Aggregators. It checks that the sum of updates
 * and count of values added for a record before and after being added are
 * equal using get_checkpoint(). It also checks the same for buckets, in
 * get_boundaries(), and counts for buckets, in get_counts().
 */
TEST(PrometheusCollector, AddMetricDataWithSketchRecordsSuccessfully)
{
  PrometheusCollector collector;

  // number of records to create
  int num_records = 2;

  // construct a collection of records with SketchAggregators and double
  std::vector<metric_sdk::Record> records =
      CreateRecords<double>(num_records, metric_sdk::AggregatorKind::Sketch);

  // add records to collection
  collector.AddMetricData(records);

  // Collection size should be the same as the size
  // of the records collection passed to addMetricData()
  ASSERT_EQ(collector.GetCollection().size(), records.size());

  // check values of records created vs records from metricsToCollect,
  // accessed by getCollection()

  for (int i = 0; i < num_records; i++)
  {
    metric_sdk::Record before = records[i];
    metric_sdk::Record after  = collector.GetCollection()[i];

    ASSERT_EQ(before.GetName(), after.GetName());

    ASSERT_EQ(before.GetDescription(), after.GetDescription());

    ASSERT_EQ(before.GetLabels(), after.GetLabels());

    auto before_agg_var = before.GetAggregator();
    auto before_agg = nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(before_agg_var);

    auto after_agg_var = after.GetAggregator();
    auto after_agg     = nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(after_agg_var);

    ASSERT_EQ(before_agg->get_checkpoint().size(), after_agg->get_checkpoint().size());
    for (size_t i = 0; i < before_agg->get_checkpoint().size(); i++)
    {
      ASSERT_EQ(before_agg->get_checkpoint()[i], after_agg->get_checkpoint()[i]);
    }
    for (size_t i = 0; i < before_agg->get_boundaries().size(); i++)
    {
      ASSERT_EQ(before_agg->get_boundaries()[i], after_agg->get_boundaries()[i]);
    }
    for (size_t i = 0; i < before_agg->get_counts().size(); i++)
    {
      ASSERT_EQ(before_agg->get_counts()[i], after_agg->get_counts()[i]);
    }
  }
}

/**
 * AddMetricData() should be able to successfully add a collection
 * of Records with Histogram Aggregators. It checks that the sum of
 * updates, number of updates, boundaries, and counts for each bucket
 * for the aggregator of a record before and after AddMetricData()
 * is called are equal.
 */
TEST(PrometheusCollector, AddMetricDataWithHistogramRecordsSuccessfully)
{
  PrometheusCollector collector;

  // number of records to create
  int num_records = 2;

  // construct a collection of records with HistogramAggregators and float
  std::vector<metric_sdk::Record> records =
      CreateRecords<float>(num_records, metric_sdk::AggregatorKind::Histogram);

  // add records to collection
  collector.AddMetricData(records);

  // Collection size should be the same as the size
  // of the records collection passed to addMetricData()
  ASSERT_EQ(collector.GetCollection().size(), records.size());

  // check values of records created vs records from metricsToCollect,
  // accessed by getCollection()

  for (int i = 0; i < num_records; i++)
  {
    metric_sdk::Record before = records[i];
    metric_sdk::Record after  = collector.GetCollection()[i];

    ASSERT_EQ(before.GetName(), after.GetName());

    ASSERT_EQ(before.GetDescription(), after.GetDescription());

    ASSERT_EQ(before.GetLabels(), after.GetLabels());

    auto before_agg_var = before.GetAggregator();
    auto before_agg = nostd::get<std::shared_ptr<metric_sdk::Aggregator<float>>>(before_agg_var);

    auto after_agg_var = after.GetAggregator();
    auto after_agg     = nostd::get<std::shared_ptr<metric_sdk::Aggregator<float>>>(after_agg_var);

    ASSERT_EQ(before_agg->get_checkpoint().size(), after_agg->get_checkpoint().size());
    for (size_t i = 0; i < before_agg->get_checkpoint().size(); i++)
    {
      ASSERT_EQ(before_agg->get_checkpoint()[i], after_agg->get_checkpoint()[i]);
    }
    for (size_t i = 0; i < before_agg->get_boundaries().size(); i++)
    {
      ASSERT_EQ(before_agg->get_boundaries()[i], after_agg->get_boundaries()[i]);
    }
    for (size_t i = 0; i < before_agg->get_counts().size(); i++)
    {
      ASSERT_EQ(before_agg->get_counts()[i], after_agg->get_counts()[i]);
    }
  }
}

/**
 * AddMetricData() should be able to successfully add a collection
 * of Records with Exact Aggregators. If the Exact Aggregator is in
 * quantile mode, it will check quantiles at selected values of 0, 0.25,
 * 0.5, 0.75, and 1. If not, it will check the vector of checkpointed
 * values in get_checkpoint().
 */
TEST(PrometheusCollector, AddMetricDataWithExactRecordsSuccessfully)
{
  PrometheusCollector collector;

  // number of records to create
  int num_records = 1;

  // construct a collection of a single record with a quantile
  // estimation ExactAggregator and double
  std::vector<metric_sdk::Record> records =
      CreateRecords<double>(num_records, metric_sdk::AggregatorKind::Exact, true);

  // add records to collection
  collector.AddMetricData(records);

  // construct a collection of a single record with an in-order
  // ExactAggregator and double
  records = CreateRecords<double>(num_records, metric_sdk::AggregatorKind::Exact, false);

  // add records to collection
  collector.AddMetricData(records);

  // Collection size should be the same as the size
  // of the records collection passed to addMetricData()
  ASSERT_EQ(collector.GetCollection().size(), records.size() * 2);

  // check values of records created vs records from metricsToCollect,
  // accessed by getCollection()

  for (int i = 0; i < num_records; i++)
  {
    metric_sdk::Record before = records[i];
    metric_sdk::Record after  = collector.GetCollection()[i];

    ASSERT_EQ(before.GetName(), after.GetName());

    ASSERT_EQ(before.GetDescription(), after.GetDescription());

    ASSERT_EQ(before.GetLabels(), after.GetLabels());

    auto before_agg_var = before.GetAggregator();
    auto before_agg = nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(before_agg_var);

    auto after_agg_var = after.GetAggregator();
    auto after_agg     = nostd::get<std::shared_ptr<metric_sdk::Aggregator<double>>>(after_agg_var);

    if (before_agg->get_quant_estimation() && after_agg->get_quant_estimation())
    {
      for (double i = 0; i <= 1;)
      {
        ASSERT_EQ(before_agg->get_quantiles(i), after_agg->get_quantiles(i));
        i += 0.25;
      }
    }
    else
    {
      ASSERT_EQ(before_agg->get_checkpoint().size(), after_agg->get_checkpoint().size());
      for (size_t i = 0; i < before_agg->get_checkpoint().size(); i++)
      {
        ASSERT_EQ(before_agg->get_checkpoint()[i], after_agg->get_checkpoint()[i]);
      }
    }
  }
}

TEST(PrometheusCollector, AddMetricDataDoesNotAddWithInsufficentSpace)
{
  PrometheusCollector collector;

  // number of records to create
  int num_records = collector.GetMaxCollectionSize() - 5;

  // construct a collection close to max capacity
  std::vector<metric_sdk::Record> records =
      CreateRecords<double>(num_records, metric_sdk::AggregatorKind::Counter);

  collector.AddMetricData(records);

  // Check if all the records have been added
  ASSERT_EQ(collector.GetCollection().size(), num_records);

  // Try adding the same collection of records again to
  // metricsToCollect.
  collector.AddMetricData(records);

  // Check that the number of records in metricsToCollect
  // has not changed.
  ASSERT_EQ(collector.GetCollection().size(), num_records);
}

TEST(PrometheusCollector, AddMetricDataDoesNotAddBadIndividualRecords)
{
  PrometheusCollector collector;

  // number of records to create
  int num_records = 5;

  // construct a collection with the specified number of records
  std::vector<metric_sdk::Record> records =
      CreateRecords<double>(num_records, metric_sdk::AggregatorKind::Counter);

  // add records to collection
  collector.AddMetricData(records);

  // Check if all the records have been added
  ASSERT_EQ(collector.GetCollection().size(), num_records);

  // Creates a bad record, with a nullptr aggregator and adds
  // it to the colelction of records
  std::string name        = "bad_record";
  std::string description = "nullptr_agg";
  std::string labels      = "{label1:v1}";
  std::shared_ptr<metric_sdk::Aggregator<int>> aggregator;
  metric_sdk::Record bad_record{name, description, labels, aggregator};

  records.push_back(bad_record);

  // add records to collection
  collector.AddMetricData(records);

  // Check if all the records except the bad
  // record have been added; the number of records added
  // should be twice the original number of records
  // epecified to be created
  ASSERT_EQ(collector.GetCollection().size(), num_records * 2);
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
  int num_records = 2;
  auto records    = CreateRecords<int>(num_records, metric_sdk::AggregatorKind::Counter);
  collector.AddMetricData(records);

  // the collection should not be empty now
  ASSERT_EQ(collector.GetCollection().size(), num_records);

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

  // construct a collection to add metric records
  int num_records = 1;
  auto records    = CreateRecords<int>(num_records, metric_sdk::AggregatorKind::Counter);
  collector.AddMetricData(records);

  // the collection should not be empty now
  ASSERT_EQ(collector.GetCollection().size(), num_records);
  auto collected = collector.Collect();

  ASSERT_EQ(collected.size(), num_records);

  auto metric_family = collected[0];

  // Collect function really collects a vector of MetricFamily
  ASSERT_EQ(metric_family.name, "record_0");
  ASSERT_EQ(metric_family.help, "record 0 for test purpose");
  ASSERT_EQ(metric_family.type, prometheus_client::MetricType::Counter);
  ASSERT_EQ(metric_family.metric.size(), 1);
  ASSERT_DOUBLE_EQ(metric_family.metric[0].counter.value, 15);
}

/**
 * Concurrency Test 1: After adding data concurrently, the intermediate collection should
 * contain all data from all threads.
 */
TEST(PrometheusCollector, ConcurrencyAddingRecords)
{
  PrometheusCollector collector;

  // construct a collection to add metric records
  int num_records = 2;
  std::vector<metric_sdk::Record> records1 =
      CreateRecords<int>(num_records, metric_sdk::AggregatorKind::Counter);

  std::vector<metric_sdk::Record> records2 =
      CreateRecords<int>(num_records, metric_sdk::AggregatorKind::Gauge);

  std::thread first(&PrometheusCollector::AddMetricData, std::ref(collector), std::ref(records1));
  std::thread second(&PrometheusCollector::AddMetricData, std::ref(collector), std::ref(records2));

  first.join();
  second.join();

  ASSERT_EQ(collector.GetCollection().size(), 4);
}

/**
 * Concurrency Test 2: After adding data concurrently and collecting, the intermediate collection
 * should be empty, and all data are collected in the result vector.
 */
TEST(PrometheusCollector, ConcurrentlyAddingAndThenCollecting)
{
  PrometheusCollector collector;

  // construct a collection to add metric records
  int num_records = 2;
  std::vector<metric_sdk::Record> records1 =
      CreateRecords<int>(num_records, metric_sdk::AggregatorKind::Counter);

  std::vector<metric_sdk::Record> records2 =
      CreateRecords<int>(num_records, metric_sdk::AggregatorKind::Gauge);

  std::thread first(&PrometheusCollector::AddMetricData, std::ref(collector), std::ref(records1));
  std::thread second(&PrometheusCollector::AddMetricData, std::ref(collector), std::ref(records2));
  first.join();
  second.join();

  auto collect_future = std::async(&PrometheusCollector::Collect, std::ref(collector));
  auto res            = collect_future.get();

  ASSERT_EQ(collector.GetCollection().size(), 0);
  ASSERT_EQ(res.size(), 4);
}

/**
 * Concurrency Test 3: Concurrently adding and collecting. We don't know when the collect function
 * is called, but all data entries are either collected or left in the collection.
 */
TEST(PrometheusCollector, ConcurrentlyAddingAndCollecting)
{
  PrometheusCollector collector;

  // construct a collection to add metric records
  int num_records = 2;
  std::vector<metric_sdk::Record> records1 =
      CreateRecords<int>(num_records, metric_sdk::AggregatorKind::Counter);

  std::vector<metric_sdk::Record> records2 =
      CreateRecords<int>(num_records, metric_sdk::AggregatorKind::Gauge);

  std::thread first(&PrometheusCollector::AddMetricData, std::ref(collector), std::ref(records1));
  std::thread second(&PrometheusCollector::AddMetricData, std::ref(collector), std::ref(records2));
  auto collect_future = std::async(&PrometheusCollector::Collect, std::ref(collector));

  first.join();
  second.join();

  auto res = collect_future.get();

  // the size of collection can be 0, 2, 4, because we don't know when the collect()
  // is really called. However, we claim that if the data in the collection is collected,
  // they must be in the res. So res.size() + collection.size() must be the total number
  // of data records we generated.
  ASSERT_EQ(res.size() + collector.GetCollection().size(), 4);
}

/**
 * Concurrency Test 4: Concurrently adding then concurrently collecting. We don't know which
 * collecting thread fetches all data, but either one should succeed.
 */
TEST(PrometheusCollector, ConcurrentlyAddingAndConcurrentlyCollecting)
{
  PrometheusCollector collector;

  // construct a collection to add metric records
  int num_records = 2;
  std::vector<metric_sdk::Record> records1 =
      CreateRecords<int>(num_records, metric_sdk::AggregatorKind::Counter);

  std::vector<metric_sdk::Record> records2 =
      CreateRecords<int>(num_records, metric_sdk::AggregatorKind::Gauge);

  // concurrently adding
  std::thread first(&PrometheusCollector::AddMetricData, std::ref(collector), std::ref(records1));
  std::thread second(&PrometheusCollector::AddMetricData, std::ref(collector), std::ref(records2));
  first.join();
  second.join();

  // after adding, then concurrently consuming
  auto collect_future1 = std::async(&PrometheusCollector::Collect, std::ref(collector));
  auto collect_future2 = std::async(&PrometheusCollector::Collect, std::ref(collector));
  auto res1            = collect_future1.get();
  auto res2            = collect_future2.get();

  // all added data must be collected in either res1 or res2
  ASSERT_EQ(res1.size() + res2.size(), 4);
}

OPENTELEMETRY_END_NAMESPACE
#endif
