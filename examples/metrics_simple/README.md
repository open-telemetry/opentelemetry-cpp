# Simple Metrics Example

This example initializes the metrics pipeline with 2 different instrument types.
Here are more detailed explanations of each part.

1: Initialize an exporter and a reader. In this case, we initialize an OStream
Exporter which will print to stdout by default.
The reader periodically collects metrics from the collector and exports them.

```cpp
std::unique_ptr<metric_sdk::MetricExporter> exporter{new exportermetrics::OStreamMetricExporter};
std::unique_ptr<metric_sdk::MetricReader> reader{
    new metric_sdk::PeriodicExportingMetricReader(std::move(exporter), options)};
```

2: Initialize a MeterProvider and add the reader.
We will use this to obtain Meter objects in the future.

```cpp
auto provider = std::shared_ptr<metrics_api::MeterProvider>(new opentelemetry::metrics::MeterProvider());
auto p = std::static_pointer_cast<metric_sdk::MeterProvider>(provider);
p->AddMetricReader(std::move(reader));
```

3: Create and add a view to the provider.

```cpp
std::unique_ptr<metric_sdk::InstrumentSelector> instrument_selector{
    new metric_sdk::InstrumentSelector(metric_sdk::InstrumentType::kCounter, "name_counter")};
std::unique_ptr<metric_sdk::MeterSelector> meter_selector{
    new metric_sdk::MeterSelector(name, version, schema)};
std::unique_ptr<metric_sdk::View> sum_view{
    new metric_sdk::View{name, "description", metric_sdk::AggregationType::kSum}};
p->AddView(std::move(instrument_selector), std::move(meter_selector), std::move(sum_view));
```

4: Then create a
[Counter](https://github.com/open-telemetry/opentelemetry-specification/blob/main/specification/metrics/api.md#counter)
instrument from it. Every Meter pointer returned by the
MeterProvider points to the same Meter. This means that the Meter will be able
to combine metrics captured from different functions without having to
constantly pass the Meter around the library.

```cpp
nostd::shared_ptr<metrics_api::Meter> meter = provider->GetMeter(name, "1.2.0");
auto double_counter = meter->CreateDoubleCounter(counter_name);
// Create a label set which annotates metric values
std::map<std::string, std::string> labels = {{"key", "value"}};
auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};
double_counter->Add(val, labelkv);
```

5: To use histogram instrument, a view with proper `InstrumentType` and `AggregationType`
has to be added to the provider.

```cpp
std::unique_ptr<metric_sdk::InstrumentSelector> histogram_instrument_selector{
    new metric_sdk::InstrumentSelector(metric_sdk::InstrumentType::kHistogram, "histogram_name")};
std::unique_ptr<metric_sdk::MeterSelector> histogram_meter_selector{
    new metric_sdk::MeterSelector(name, version, schema)};
std::unique_ptr<metric_sdk::View> histogram_view{
    new metric_sdk::View{name, "description", metric_sdk::AggregationType::kHistogram}};
p->AddView(std::move(histogram_instrument_selector), std::move(histogram_meter_selector),
            std::move(histogram_view));

auto histogram_counter = meter->CreateDoubleHistogram("histogram_name");
auto context = opentelemetry::context::Context{};
histogram_counter->Record(val, labelkv, context);
```

See [CONTRIBUTING.md](../../CONTRIBUTING.md) for instructions on building and
running the example.

## Additional Documentation

[API
Design](https://github.com/open-o11y/docs/blob/master/cpp-metrics/api-design.md)

[SDK
Design](https://github.com/open-o11y/docs/blob/master/cpp-metrics/sdk-design.md)

[OStreamExporters
Design](https://github.com/open-o11y/docs/blob/master/cpp-ostream/ostream-exporter-design.md)

[OpenTelemetry C++ Metrics
Overview](https://github.com/open-o11y/docs/blob/master/cpp-metrics/README.md)
