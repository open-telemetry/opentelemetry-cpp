# Simple Metrics Example

In this example, the application in `main.cc` initializes the metrics pipeline
and shows 3 different ways of updating instrument values. Here are more detailed
explanations of each part.

1: Initialize a MeterProvider. We will use this to obtain Meter objects in the
future.

`auto provider = shared_ptr<MeterProvider>(new MeterProvider);`

2: Set the MeterProvider as the default instance for the library. This ensures
that we will have access to the same MeterProvider across our application.

`Provider::SetMeterProvider(provider);`

3: Obtain a meter from this meter provider. Every Meter pointer returned by the
MeterProvider points to the same Meter. This means that the Meter will be able
to combine metrics captured from different functions without having to
constantly pass the Meter around the library.

`shared_ptr<Meter> meter = provider→GetMeter("Test");`

4: Initialize an exporter and processor. In this case, we initialize an OStream
Exporter which will print to stdout by default. The Processor is an
UngroupedProcessor which doesn’t filter or group captured metrics in any way.
The false parameter indicates that this processor will send metric deltas rather
than metric cumulatives.

```cpp
unique_ptr<MetricsExporter> exporter = unique_ptr<MetricsExporter>(new OStreamMetricsExporter);
shared_ptr<MetricsProcessor> processor = shared_ptr<MetricsProcessor>(new UngroupedMetricsProcessor(false));
```

5: Pass the meter, exporter, and processor into the controller. Since this is a
push controller, a collection interval parameter (in seconds) is also taken. At
each collection interval, the controller will request data from all of the
instruments in the code and export them. Start the controller to begin the
metrics pipeline.

`metrics_sdk::PushController controller(meter, std::move(exporter), processor,
5);` `controller.start();`

6: Instrument code with synchronous and asynchronous instrument. These
instruments can be placed in areas of interest to collect metrics and are
created by the meter. Synchronous instruments are updated whenever the user
desires with a value and label set. Calling add on a counter instrument for
example will increase its value.  Asynchronous instruments can be updated the
same way, but are intended to receive updates from a callback function. The
callback below observes a value of 1. The user never has to call this function
as it is automatically called by the controller.

```cpp
// Observer callback function
void SumObserverCallback(metrics_api::ObserverResult<int> result){
    std::map<std::string, std::string> labels = {{"key", "value"}};
    auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};
    result.observe(1,labelkv);
}

// Create new instruments
auto ctr= meter->NewIntCounter("Counter","none", "none", true);
auto obs= meter->NewIntSumObserver("Counter","none", "none", true, &SumObserverCallback);

// Create a label set which annotates metric values
std::map<std::string, std::string> labels = {{"key", "value"}};
auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};

// Capture data from instruments.  Note that the asynchronous instrument is updates
// automatically though its callback at the collection interval.  Additional measurments
// can be made through calls to its observe function.
ctr->add(5, labelkv);

```

7: Stop the controller once the program finished. This ensures that any metrics
inside the pipeline are properly exported. Otherwise, some metrics may be
destroyed in cleanup.

`controller.stop();`

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
