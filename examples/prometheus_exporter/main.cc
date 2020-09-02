#include "opentelemetry/exporters/prometheus/prometheus_collector.h"
#include "opentelemetry/exporters/prometheus/prometheus_exporter.h"
#include "opentelemetry/sdk/metrics/aggregator/counter_aggregator.h"
#include "opentelemetry/sdk/metrics/aggregator/exact_aggregator.h"
#include "opentelemetry/sdk/metrics/aggregator/gauge_aggregator.h"
#include "opentelemetry/sdk/metrics/aggregator/histogram_aggregator.h"
#include "opentelemetry/sdk/metrics/aggregator/min_max_sum_count_aggregator.h"
#include "opentelemetry/sdk/metrics/aggregator/sketch_aggregator.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

namespace prom_exp   = opentelemetry::exporter::prometheus;
namespace metric_sdk = opentelemetry::sdk::metrics;
namespace metric_api = opentelemetry::metrics;

namespace
{
  template <typename T>
  metric_sdk::Record get_record(const std::string &type,
                                int version,
                                const std::string &label,
                                std::shared_ptr<metric_sdk::Aggregator<T>> aggregator)
  {
    std::string name = "test-" + type + "-metric-record-v_" + std::to_string(version) + ".0";
    std::string desc = "this is a test " + type + " metric record";
    metric_sdk::Record record(name, desc, label, aggregator);
    return record;
  }

  template <typename T>
  void do_example(const std::shared_ptr<metric_sdk::Aggregator<T>> &aggregator,
                  std::vector<metric_sdk::Record> collection,
                  const metric_sdk::Record &record,
                  prom_exp::PrometheusExporter &exporter)
  {
    int counter = 0;
    while (true)
    {
      int val = (rand() % 100) + 1;
      aggregator->update(val);

      counter++;
      std::this_thread::sleep_for(std::chrono::milliseconds(250));

      if (counter % 60 == 0)
      {
        // add record to collection and export the collection
        aggregator->checkpoint();
        collection.emplace_back(record);
        exporter.Export(collection);
        counter = 0;
      }
    }
  }

  void counter_example(prom_exp::PrometheusExporter &exporter)
  {
    auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
        new metric_sdk::CounterAggregator<int>(metric_api::InstrumentKind::Counter));

    // collection of records to export
    std::vector<metric_sdk::Record> collection;
    auto record = get_record("counter", 1, "{label-1:v1,label2:v2,}", aggregator);
    do_example(aggregator, collection, record, exporter);
  }

  void gauge_example(prom_exp::PrometheusExporter &exporter)
  {
    auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
        new metric_sdk::GaugeAggregator<int>(metric_api::InstrumentKind::Counter));

    // collection of records to export
    std::vector<metric_sdk::Record> collection;
    auto record = get_record("gauge", 1, "{label-1:v1,label2:v2,}", aggregator);
    do_example(aggregator, collection, record, exporter);
  }

  void mmsc_example(prom_exp::PrometheusExporter &exporter)
  {
    auto aggregator = std::shared_ptr<metric_sdk::Aggregator<double>>(
        new metric_sdk::MinMaxSumCountAggregator<double>(metric_api::InstrumentKind::Counter));

    // collection of records to export
    std::vector<metric_sdk::Record> collection;
    auto record = get_record("mmsc", 1, "{label-1:v1,label2:v2,}", aggregator);
    do_example(aggregator, collection, record, exporter);
  }

  void histogram_example(prom_exp::PrometheusExporter &exporter)
  {
    std::vector<double> boundaries{10, 20, 30, 40, 50};
    auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
        new metric_sdk::HistogramAggregator<int>(metric_api::InstrumentKind::Counter, boundaries));

    // collection of records to export
    std::vector<metric_sdk::Record> collection;
    auto record = get_record("histogram", 1, "{label-1:v1,label2:v2,}", aggregator);
    do_example(aggregator, collection, record, exporter);
  }

  void exact_example(prom_exp::PrometheusExporter &exporter)
  {
    auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
        new metric_sdk::ExactAggregator<int>(metric_api::InstrumentKind::Counter, true));

    // collection of records to export
    std::vector<metric_sdk::Record> collection;
    auto record = get_record("exact", 1, "{label-1:v1,label2:v2,}", aggregator);
    do_example(aggregator, collection, record, exporter);
  }

  void sketch_example(prom_exp::PrometheusExporter &exporter)
  {
    auto aggregator = std::shared_ptr<metric_sdk::Aggregator<int>>(
        new metric_sdk::SketchAggregator<int>(metric_api::InstrumentKind::Counter, true));

    // collection of records to export
    std::vector<metric_sdk::Record> collection;
    auto record = get_record("sketch", 1, "{label-1:v1,label2:v2,}", aggregator);
    do_example(aggregator, collection, record, exporter);
  }
}
int main(int argc, char** argv)
{
  std::string agg(argv[1]);

  std::string address = "localhost:8080";
  prom_exp::PrometheusExporter exporter{address};

  std::cout << "Prometheus Exporter example program running on " << address << "...\n";
  std::cout << "Running " << agg << " example program...\n";
  
  if (agg == "counter")
  {
    counter_example(exporter);
  }
  else if (agg == "gauge")
  {
    gauge_example(exporter);
  }
  else if (agg == "mmsc")
  {
    mmsc_example(exporter);
  }
  else if (agg == "histogram")
  {
    histogram_example(exporter);
  }
  else if (agg == "exact")
  {
    exact_example(exporter);
  }
  else if (agg == "sketch")
  {
    sketch_example(exporter);
  }

  return 0;
}