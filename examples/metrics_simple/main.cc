// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW
#  include "opentelemetry/_metrics/provider.h"
#  include "opentelemetry/exporters/ostream/metrics_exporter.h"
#  include "opentelemetry/sdk/_metrics/controller.h"
#  include "opentelemetry/sdk/_metrics/meter.h"
#  include "opentelemetry/sdk/_metrics/meter_provider.h"
#  include "opentelemetry/sdk/_metrics/ungrouped_processor.h"

namespace metric_sdk      = opentelemetry::sdk::metrics;
namespace nostd           = opentelemetry::nostd;
namespace common          = opentelemetry::common;
namespace exportermetrics = opentelemetry::exporter::metrics;
namespace metrics_api     = opentelemetry::metrics;

int main()
{
  // Initialize and set the global MeterProvider
  auto provider = nostd::shared_ptr<metrics_api::MeterProvider>(new metric_sdk::MeterProvider);
  metrics_api::Provider::SetMeterProvider(provider);

  // Get the Meter from the MeterProvider
  nostd::shared_ptr<metrics_api::Meter> meter = provider->GetMeter("Test", "0.1.0");

  // Create the controller with Stateless Metrics Processor
  metric_sdk::PushController ControllerStateless(
      meter,
      std::unique_ptr<metric_sdk::MetricsExporter>(new exportermetrics::OStreamMetricsExporter),
      std::shared_ptr<metric_sdk::MetricsProcessor>(
          new metric_sdk::UngroupedMetricsProcessor(false)),
      .05);

  // Create and instrument
  auto intupdowncounter = meter->NewIntUpDownCounter("UpDownCounter", "None", "none", true);
  auto intcounter       = meter->NewIntCounter("Counter", "none", "none", true);

  // Create a labelset
  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};

  // Create arrays of instrument and values to add to them
  metrics_api::SynchronousInstrument<int> *iinstr_arr[] = {intupdowncounter.get(),
                                                           intcounter.get()};
  int ivalues_arr[]                                     = {10, 5};

  // Change the arrays to be nostd::spans
  nostd::span<metrics_api::SynchronousInstrument<int> *> instrument_span{iinstr_arr};
  nostd::span<const int, 2> instrument_values{ivalues_arr};

  /**
   * First way of updating an instrument, RecordBatch. We can update multiple instruments at once by
   * using a span of instruments and a span of values. This RecordBatch will update the ith
   * instrument with the ith value.
   **/
  std::cout << "Example 1" << std::endl;
  ControllerStateless.start();

  // Updating multiple instruments with the same labelset
  meter->RecordIntBatch(labelkv, instrument_span, instrument_values);

  ControllerStateless.stop();
  /**
   * Second way of updating an instrument, bind then add. In this method the user binds an
   *instrument to a labelset Then add to the bounded instrument, then unbind.
   **/
  std::cout << "Example 2" << std::endl;
  ControllerStateless.start();

  auto boundintupdowncounter = intupdowncounter->bindUpDownCounter(labelkv);
  boundintupdowncounter->add(50);
  boundintupdowncounter->unbind();

  ControllerStateless.stop();
  /**
   * The Third and final way is to add a value with a labelset at the same time. This also shows
   * The difference between using a Stateless and Stateful Processor
   */

  // Start exporting from the Controller with Stateless Processor
  std::cout << "-----"
            << " Stateless Processor "
            << "-----" << std::endl;
  ControllerStateless.start();
  for (int i = 0; i < 20; i++)
  {
    intupdowncounter->add(i, labelkv);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  ControllerStateless.stop();

  // Do the same thing for stateful to see the difference
  metric_sdk::PushController ControllerStateful(
      meter,
      std::unique_ptr<metric_sdk::MetricsExporter>(new exportermetrics::OStreamMetricsExporter),
      std::shared_ptr<metric_sdk::MetricsProcessor>(
          new metric_sdk::UngroupedMetricsProcessor(true)),
      .05);

  // Start exporting from the Controller with Stateful Processor
  std::cout << "-----"
            << " Stateful Processor "
            << "-----" << std::endl;
  ControllerStateful.start();
  for (int i = 0; i < 20; i++)
  {
    intupdowncounter->add(i, labelkv);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  ControllerStateful.stop();
}
#else
int main()
{
  // empty
}
#endif
