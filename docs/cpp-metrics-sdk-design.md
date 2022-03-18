# Metrics SDK Design

## Design Tenets

* Reliability
  * The Metrics API and SDK should be “reliable,” meaning that metrics data will
    always be accounted for. It will get back to the user or an error will be
    logged.  Reliability also entails that the end-user application will never
    be blocked.  Error handling will therefore not interfere with the execution
    of the instrumented program.  The library may “fail fast” during the
    initialization or configuration path however.
  * Thread Safety
    * As with the Tracer API and SDK, thread safety is not guaranteed on all
      functions and will be explicitly mentioned in documentation for functions
      that support concurrent calling.  Generally, the goal is to lock functions
      which change the state of library objects (incrementing the value of a
      Counter or adding a new Observer for example) or access global memory.  As
      a performance consideration, the library strives to hold locks for as
      short a duration as possible to avoid lock contention concerns.  Calls to
      create instrumentation may not be thread-safe as this is expected to occur
      during initialization of the program.
* Scalability
  * As OpenTelemetry is a distributed tracing system, it must be able to operate
    on sizeable systems with predictable overhead growth.  A key requirement of
    this is that the library does not consume unbounded memory resource.
* Security
  * Currently security is not a key consideration but may be addressed at a
    later date.

## SDK Data Path Diagram

<!-- [//]: # ![Data Path Diagram](../images/DataPath.png) -->

This is the control path our implementation of the metrics SDK will follow.
There are five main components: The controller, accumulator, aggregators,
processor, and exporter. Each of these components will be further elaborated on.

## API Class Implementations

### MeterProvider Class

The singleton global `MeterProvider` can be used to obtain a global Meter by
calling `global.GetMeter(name,version)` which calls `GetMeter()` on the
initialized global `MeterProvider`.

**Global Meter Provider:**

The API should support a global `MeterProvider`. When a global instance is
supported, the API must ensure that `Meter` instances derived from the global
`MeterProvider` are initialized after the global SDK implementation is first
initialized.

A `MeterProvider` interface must support a
`global.SetMeterProvider(MeterProvider)` function which installs the SDK
implementation of the `MeterProvider` into the API.

**Obtaining a Meter from MeterProvider:**

**`GetMeter(name, version)` method must be supported**

* Expects 2 string arguments:
  * name (required): identifies the instrumentation library.
  * version (optional): specifies the version of the instrumenting library (the
    library injecting OpenTelemetry calls into the code).

#### Implementation

The Provider class offers static functions to both get and set the global
MeterProvider. Once a user sets the MeterProvider, it will replace the default
No-op implementation stored as a private variable and persist for the remainder
of the program’s execution. This pattern imitates the TracerProvider used in the
Tracing portion of this SDK.

```cpp
# meter_provider.cc
class MeterProvider
{
public:
  /*
   * Get Meter
   *
   * Gets or creates a named meter instance.
   *
   * Arguments:
   * library_name, the name of the instrumenting library.
   * library_version, the version of the instrumenting library (OPTIONAL).
   */
  nostd::shared_ptr<Meter> GetMeter(nostd::string_view library_name,
                                     nostd::string_view library_version = "") {

    // Create an InstrumentationInfo object which holds the library name and version.
    // Call the Meter constructor with InstrumentationInfo.
    InstrumentationInfo instrumentationInfo;
    instrumentationInfo.SetName(library_name);
    if library_version:
      instrumentationInfo.SetVersion(library_version);
    return nostd::shared_ptr<Meter>(Meter(this, instrumentationInfo));
  }

};
```

### Meter Class

**Metric Events:**

Metric instruments are primarily defined by their name.  Names MUST conform to
the following syntax:

* Non-empty string
* case-insensitive
* first character non-numeric, non-space, non-punctuation
* subsequent characters alphanumeric, ‘_’, ‘.’ , and ‘-’

`Meter` instances MUST return an error when multiple instruments with the same
name are registered

**The meter implementation will throw an illegal argument exception if the
user-passed `name` for a metric instrument either conflicts with the name of
another metric instrument created from the same `meter` or violates the `name`
syntax outlined above.**

Each distinctly named Meter (i.e. Meters derived from different instrumentation
libraries) MUST create a new namespace for metric instruments descending from
them.  Thus, the same instrument name can be used in an application provided
they come from different Meter instances.

**In order to achieve this, each instance of the `Meter` class will have a
container storing all metric instruments that were created using that meter.
This way, metric instruments created from different instantiations of the
`Meter` class will never be compared to one another and will never result in an
error.**

**Implementation:**

```cpp
# meter.h / meter.cc
class Meter : public API::Meter {
public:
 /*
  * Constructor for Meter class
  *
  * Arguments:
  * MeterProvider, the MeterProvider object that spawned this Meter.
  * InstrumentationInfo, the name of the instrumentation library and, optionally,
  *                      the version.
  *
  */
  explicit Meter(MeterProvider meterProvider,
                 InstrumentationInfo instrumentationInfo) {
    meterProvider_(meterProvider);
    instrumentationInfo_(instrumentationInfo);
  }

/////////////////////////Metric Instrument Constructors////////////////////////////

 /*
  * New Int Counter
  *
  * Function that creates and returns a Counter metric instrument with value
  * type int.
  *
  * Arguments:
  * name, the name of the metric instrument (must conform to the above syntax).
  * description, a brief, readable description of the metric instrument.
  * unit, the unit of metric values following the UCUM convention
  *       (https://unitsofmeasure.org/ucum.html).
  *
  */
  nostd::shared_ptr<Counter<int>> NewIntCounter(nostd::string_view name,
                                                nostd::string_view description,
                                                nostd::string_view unit,
                                                nostd::string_view enabled) {
    auto intCounter = Counter<int>(name, description, unit, enabled);
    ptr = shared_ptr<Counter<int>>(intCounter)
    int_metrics_.insert(name, ptr);
    return ptr;
  }

 /*
  * New float Counter
  *
  * Function that creates and returns a Counter metric instrument with value
  * type float.
  *
  * Arguments:
  * name, the name of the metric instrument (must conform to the above syntax).
  * description, a brief, readable description of the metric instrument.
  * unit, the unit of metric values following the UCUM convention
  *       (https://unitsofmeasure.org/ucum.html).
  *
  */
  nostd::unique_ptr<Counter<float>> NewFloatCounter(nostd::string_view name,
                                                    nostd::string_view description,
                                                    nostd::string_view unit,
                                                    nostd::string_view enabled) {
    auto floatCounter = Counter<float>(name, description, unit, enabled);
    ptr = unique_ptr<Counter<Float>>(floatCounter)
    float_metrics_.insert(name, ptr);
    return ptr;
  }

////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
//                     Repeat above two functions for all                         //
//                     six (five other) metric instruments                        //
//                     of types short, int, float, and double.                    //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////

private:
 /*
  * Collect (THREADSAFE)
  *
  * Checkpoints all metric instruments created from this meter and returns a
  * vector of records containing the name, labels, and values of each instrument.
  * This function also removes instruments that have not received updates in the
  * last collection period.
  *
  */
  std::vector<Record> Collect() {
    std::vector<Record> records;
    metrics_lock_.lock();
    for instr in ALL_metrics_:
      if instr is not enabled:
        continue
      else:
        for bound_instr in instr.BoundInstruments:
          records.push_back(Record(instr->name, instr->description,
                                   bound_instr->labels,
                                   bound_instr->GetAggregator()->Checkpoint());
    metrics_lock_.unlock();
    return records;
  }

 /*
  * Record Batch
  *
  * Allows the functionality of acting upon multiple metrics with the same set
  * of labels with a single API call. Implementations should find bound metric
  * instruments that match the key-value pairs in the labels.
  *
  * Arguments:
  * labels, labels associated with all measurements in the batch.
  * records, a KeyValueIterable containing metric instrument names such as
  *          "IntCounter" or "DoubleSumObserver" and the corresponding value
  *          to record for that metric.
  *
  */
  void RecordBatch(nostd::string_view labels,
                   nostd::KeyValueIterable values) {
    for instr in metrics:
      instr.bind(labels) // Bind the instrument to the label set
      instr.record(values.GetValue(instr.type)) // Record the corresponding value
                                                // to the instrument.
  }

  std::map<nostd::string_view, shared_ptr<SynchronousInstrument<short>>> short_metrics_;
  std::map<nostd::string_view, shared_ptr<SynchronousInstrument<short>>> int_metrics_;
  std::map<nostd::string_view, shared_ptr<SynchronousInstrument<short>>> float_metrics_;
  std::map<nostd::string_view, shared_ptr<SynchronousInstrument<short>>> double_metrics_;

  std::map<nostd::string_view, shared_ptr<AsynchronousInstrument<short>>> short_observers_;
  std::map<nostd::string_view, shared_ptr<AsynchronousInstrument<short>>> int_observers_;
  std::map<nostd::string_view, shared_ptr<AsynchronousInstrument<short>>> float_observers_;
  std::map<nostd::string_view, shared_ptr<AsynchronousInstrument<short>>> double_observers_;

  std::mutex metrics_lock_;
  unique_ptr<MeterProvider> meterProvider_;
  InstrumentationInfo instrumentationInfo_;
};
```

```cpp
# record.h
/*
 * This class is used to pass checkpointed values from the Meter
 * class, to the processor, to the exporter. This class is not
 * templated but instead uses variants in order to avoid needing
 * to template the exporters.
 *
 */
class Record
{
public:
  explicit Record(std::string name, std::string description,
                  metrics_api::BoundInstrumentKind instrumentKind,
                  std::string labels,
                  nostd::variant<Aggregator<short>, Aggregator<int>, Aggregator<float>, Aggregator<Double>> agg)
  {
    name_ = name;
    description_ = description;
    instrumentKind_ = instrumentKind;
    labels_ = labels;
    aggregator_ = aggregator;
  }

  string GetName() {return name_;}

  string GetDescription() {return description_;}

  BoundInstrumentKind GetInstrumentKind() {return instrumentKind_;}

  string GetLabels() {return labels_;}

  nostd::variant<Aggregator<short>, Aggregator<int>, Aggregator<float>, Aggregator<Double>> GetAggregator() {return aggregator_;}

private:
  string name_;
  string description_;
  BoundInstrumentKind instrumentKind_;
  string labels_;
  nostd::variant<Aggregator<short>, Aggregator<int>, Aggregator<float>, Aggregator<Double>> aggregator_;
};
```

Metric instruments created from this Meter class will be stored in a map (or
another, similar container [needs to be nostd]) called “metrics.” This is
identical to the Python implementation and makes sense because the SDK
implementation of the `Meter` class should have a function titled
`collect_all()` that collects metrics for every instrument created from this
meter. In contrast, Java’s implementation has a `MeterSharedState` class that
contains a registry (hash map) of all metric instruments spawned from this
meter. However, since each `Meter` has its own unique instruments it is easier
to store the instruments in the meter itself.

The SDK implementation of the `Meter` class will contain a function called
`collect_all()` that will collect the measurements from each metric stored in
the `metrics` container. The implementation of this class acts as the
accumulator in the SDK specification.

**Pros of this implementation:**

* Different constructors and overloaded template calls to those constructors for
  the various metric instruments allows us to forego much of the code
  duplication involved in supporting various types.
* Storing the metric instruments created from this meter directly in the meter
  object itself allows us to implement the collect_all method without creating a
  new class that contains the meter state and instrument registry.

**Cons of this implementation:**

* Different constructors for the different metric instruments means less
  duplicated code but still a lot.
* Storing the metric instruments in the Meter class means that if we have
  multiple meters, metric instruments are stored in various objects. Using an
  instrument registry that maps meters to metric instruments resolves this.
  However, we have designed our SDK to only support one Meter instance.
* Storing 8 maps in the meter class is costly. However, we believe that this is
  ok because these maps will only need to be created once, at the instantiation
  of the meter class. **We believe that these maps will not slow down the
  pipeline in any meaningful way**

**The SDK implementation of the `Meter` class will act as the Accumulator
mentioned in the SDK specification.**

## **Metric Instrument Class**

Metric instruments capture raw measurements of designated quantities in
instrumented applications. All measurements captured by the Metrics API are
associated with the instrument which collected that measurement.

### Metric Instrument Data Model

Each instrument must have enough information to meaningfully attach its measured
values with a process in the instrumented application. As such, metric
instruments contain the following fields

* name (string) — Identifier for this metric instrument.
* description (string) — Short description of what this instrument is capturing.
* value_type (string or enum) — Determines whether the value tracked is an int64
  or double.
* meter (Meter) — The Meter instance from which this instrument was derived.
* label_keys (KeyValueIterable) — A nostd class acting as map from
  nostd::string_view to nostd::string_view.
* enabled (boolean) — Determines whether the instrument is currently collecting
  data.
* bound_instruments (key value container) — Contains the bound instruments
  derived from this instrument.

### Metric Event Data Model

Each measurement taken by a Metric instrument is a Metric event which must
contain the following information:

* timestamp (implicit) — System time when measurement was captured.
* instrument definition(strings) — Name of instrument, kind, description, and
  unit of measure
* label set (key value pairs) — Labels associated with the capture, described
  further below.
* resources associated with the SDK at startup

**Label Set:**

A key:value mapping of some kind MUST be supported as annotation each metric
event. Labels must be represented the same way throughout the API (i.e. using
the same idiomatic data structure) and duplicates are dealt with by taking the
last value mapping.

Due to the requirement to maintain ABI stability we have chosen to implement
labels as type KeyValueIterable. Though, due to performance reasons, we may
convert to std::string internally.

**Implementation:**

A base Metric class defines the constructor and binding functions which each
metric instrument will need. Once an instrument is bound, it becomes a
BoundInstrument which extends the BaseBoundInstrument class. The
BaseBoundInstrument is what communicates with the aggregator and performs the
actual updating of values. An enum helps to organize the numerous types of
metric instruments that will be supported.

The only addition to the SDK metric instrument classes from their API
counterparts is the function GetRecords() and the private variables
std::map<std::string, BoundSynchronousInstrument> to hold bound instruments and
`Aggregator<T>` to hold the instrument's aggregator.

**For more information about the structure of metric instruments, refer to the
Metrics API Design document.**

## Metrics SDK Data Path Implementation

Note: these requirements come from a specification currently under development.
Changes and feedback are in [PR
347](https://github.com/open-telemetry/opentelemetry-specification/pull/347)
and the current document is linked
[here](https://github.com/open-telemetry/opentelemetry-specification/blob/64bbb0c611d849b90916005d7714fa2a7132d0bf/specification/metrics/sdk.md).

<!-- [//]: # ![Data Path Diagram](../images/DataPath.png) -->

### Accumulator

The Accumulator is responsible for computing aggregation over a fixed unit of
time. It essentially takes a set of captures and turns them into a quantity that
can be collected and used for meaningful analysis by maintaining aggregators for
each active instrument and each distinct label set. For example, the aggregator
for a counter must combine multiple calls to Add(increment) into a single sum.

Accumulators MUST support a `Checkpoint()` operation which saves a snapshot of
the current state for collection and a `Merge()` operation which combines the
state from multiple aggregators into one.

Calls to the Accumulator's  `Collect()` sweep through metric instruments with
un-exported updates, checkpoints their aggregators, and submits them to the
processor/exporter. This and all other accumulator operations should be
extremely efficient and follow the shortest code path possible.

Design choice: We have chosen to implement the Accumulator as the SDK
implementation of the Meter interface shown above.

### Aggregator

The term *aggregator* refers to an implementation that can combine multiple
metric updates into a single, combined state for a specific function.
Aggregators MUST support `Update()`, `Checkpoint()`, and `Merge()` operations.
`Update()` is called directly from the Metric instrument in response to a metric
event, and may be called concurrently.  The `Checkpoint()` operation is called
to atomically save a snapshot of the Aggregator. The `Merge()` operation
supports dimensionality reduction by combining state from multiple aggregators
into a single Aggregator state.

The SDK must include the Counter aggregator which maintains a sum and the gauge
aggregator which maintains last value and timestamp. In addition, the SDK should
include MinMaxSumCount, Sketch, Histogram, and Exact aggregators All operations
should be atomic in languages that support them.

```cpp
# aggregator.cc
class Aggregator {
public:
    explicit Aggregator() {
        self.current_ = nullptr
        self.checkpoint_ = nullptr
    }

   /*
    * Update
    *
    * Updates the current value with the new value.
    *
    * Arguments:
    * value, the new value to update the instrument with.
    *
    */
    virtual void Update(<T> value);

   /*
    * Checkpoint
    *
    * Stores a snapshot of the current value.
    *
    */
    virtual void Checkpoint();

   /*
    * Merge
    *
    * Combines two aggregator values. Update to most recent time stamp.
    *
    * Arguments:
    * other, the aggregator whose value to merge.
    *
    */
    virtual void Merge(Aggregator other);

    /*
     * Getters for various aggregator specific fields
     */
     virtual std::vector<T> get_value() {return current_;}
     virtual std::vector<T> get_checkpoint() {return checkpoint_;}
     virtual core::SystemTimeStamp get_timestamp() {return last_update_timestamp_;}

private:
    std::vector<T> current_;
    std::vector<T> checkpoint_;
    core::Systemtimestamp last_update_timestamp_;
};
```

```cpp
# counter_aggregator.cc
template <class T>
class CounterAggregator : public Aggregator<T> {
public:
    explicit CounterAggregator(): current(0), checkpoint(0),
                                  last_update_timestamp(nullptr){}

    void Update(T value) {
      // thread lock
      // current += value
      this->last_update_timestamp = time_ns()
    }

    void Checkpoint() {
      // thread lock
      this->checkpoint = this->current
      this->current = 0
    }

    void Merge(CounterAggregator* other) {
      // thread lock
      // combine checkpoints
      // update timestamp to now
    }
};
```

This Counter is an example Aggregator. We plan on implementing all the
Aggregators in the specification: Counter, Gauge, MinMaxSumCount, Sketch,
Histogram, and Exact.

### Processor

The Processor SHOULD act as the primary source of configuration for exporting
metrics from the SDK. The two kinds of configuration are:

1. Given a metric instrument, choose which concrete aggregator type to apply for
   in-process aggregation.
2. Given a metric instrument, choose which dimensions to export by (i.e., the
   "grouping" function).

During the collection pass, the Processor receives a full set of check-pointed
aggregators corresponding to each (Instrument, LabelSet) pair with an active
record managed by the Accumulator. According to its own configuration, the
Processor at this point determines which dimensions to aggregate for export; it
computes a checkpoint of (possibly) reduced-dimension export records ready for
export. It can be thought of as the business logic or processing phase in the
pipeline.

Change of dimensions: The user-facing metric API allows users to supply
LabelSets containing an unlimited number of labels for any metric update. Some
metric exporters will restrict the set of labels when exporting metric data,
either to reduce cost or because of system-imposed requirements. A *change of
dimensions* maps input LabelSets with potentially many labels into a LabelSet
with a fixed set of label keys. A change of dimensions eliminates labels with
keys not in the output LabelSet and fills in empty values for label keys that
are not in the input LabelSet. This can be used for different filtering options,
rate limiting, and alternate aggregation schemes. Additionally, it will be used
to prevent unbounded memory growth through capping collected data. The community
is still deciding exactly how metrics data will be pruned and this document will
be updated when a decision is made.

The following is a pseudo code implementation of a ‘simple’ Processor.

Note: Josh MacDonald is working on implementing a [‘basic’
Processor](https://github.com/jmacd/opentelemetry-go/blob/jmacd/mexport/sdk/metric/processor/simple/simple.go)
which allows for further Configuration that lines up with the specification in
Go. He will be finishing the implementation and updating the specification
within the next few weeks.

Design choice: We recommend that we implement the ‘simple’ Processor first as
apart of the MVP and then will also implement the ‘basic’ Processor later on.
Josh recommended having both for doing different processes.

```cpp
#processor.cc
class Processor {
public:

  explicit Processor(Bool stateful) {
    // stateful determines whether the processor computes deltas or lifetime changes
    // in metric values
    stateful_ = stateful;
  }

 /*
  * Process
  *
  * This function chooses which dimensions to aggregate for export.  In the
  * reference implementation, the UngroupedProcessor does not process records
  * and simple passes them along to the next step.
  *
  * Arguments:
  * record, a record containing data collected from the active Accumulator
  * in this data pipeline
  */
  void Process(Record record);

 /*
  * Checkpoint
  *
  * This function computes a new (possibly dimension-reduced) checkpoint set of
  * all instruments in the meter passed to process.
  *
  */
  Collection<Record> Checkpoint();

 /*
  * Finished Collection
  *
  * Signals to the intergrator that a collection has been completed and
  * can now be sent for export.
  *
  */
  Error FinishedCollection();

 /*
  * Aggregator For
  *
  * Returns the correct aggregator type for a given metric instrument.  Used in
  * the instrument constructor to select which aggregator to use.
  *
  * Arguments:
  * kind, the instrument type asking to be assigned an aggregator
  *
  */
  Aggregator AggregatorFor(MetricKind kind);

private:
  Bool stateful_;
  Batch batch_;
};
```

### Controller

Controllers generally are responsible for binding the Accumulator, the
Processor, and the Exporter. The controller initiates the collection and export
pipeline and manages all the moving parts within it. It also governs the flow of
data through the SDK components. Users interface with the controller to begin
collection process.

Once the decision has been made to export, the controller must call `Collect()`
on the Accumulator, then read the checkpoint from the Processor, then invoke the
Exporter.

Java’s IntervalMetricReader class acts as a parallel to the controller. The user
gets an instance of this class, sets the configuration options (like the tick
rate) and then the controller takes care of the collection and exporting of
metric data from instruments the user defines.

There are two different controllers: Push and Pull. The “Push” Controller will
establish a periodic timer to regularly collect and export metrics. A “Pull”
Controller will await a pull request before initiating metric collection.

We recommend implementing the PushController as the initial implementation of
the Controller. This Controller is the base controller in the specification. We
may also implement the PullController if we have the time to do it.

```cpp
#push_controller.cc
class PushController {

    explicit PushController(Meter meter, Exporter exporter,
                            int period, int timeout) {
        meter_ = meter;
        exporter_ = exporter();
        period_ = period;
        timeout_ = timeout;
        provider_ = NewMeterProvider(accumulator);
    }

   /*
    * Provider
    *
    * Returns the MeterProvider stored by this controller.
    *
    */
    MeterProvider Provider {
        return this.provider_;
    }

   /*
    * Start (THREAD SAFE)
    *
    * Begins a ticker that periodically collects and exports metrics with a
    * configured interval.
    *
    */
    void Start();

   /*
    * Stop (THREAD SAFE)
    *
    * Waits for collection interval to end then exports metrics one last time
    * before returning.
    *
    */
    void Stop();

   /*
    * Run
    *
    * Used to wait between collection intervals.
    *
    */
    void run();

   /*
    * Tick (THREAD SAFE)
    *
    * Called at regular intervals, this function collects all values from the
    * member variable meter_, then sends them to the processor_ for
    * processing. After the records have been processed they are sent to the
    * exporter_ to be exported.
    *
    */
    void tick();

private:
    mutex lock_;
    Meter meter_;
    MeterProvider provider_;
    Exporter exporter_;
    int period_;
    int timeout_;
};
```

### Exporter

The exporter SHOULD be called with a checkpoint of finished (possibly
dimensionally reduced) export records. Most configuration decisions have been
made before the exporter is invoked, including which instruments are enabled,
which concrete aggregator types to use, and which dimensions to aggregate by.

There is very little left for the exporter to do other than format the metric
updates into the desired format and send them on their way.

Design choice: Our idea is to take the simple trace example
[OStreamSpanExporter](https://github.com/open-telemetry/opentelemetry-cpp/blob/main/examples/simple/main.cc)
and add Metric functionality to it. This will allow us to verify that what we
are implementing in the API and SDK works as intended. The exporter will go
through the different metric instruments and print the value stored in their
aggregators to stdout, **for simplicity only Sum is shown here, but all
aggregators will be implemented**.

```cpp
# stdout_exporter.cc
class StdoutExporter: public exporter {
   /*
    * Export
    *
    * For each checkpoint in checkpointSet, print the value found in their
    * aggregator to stdout. Returns an ExportResult enum error code.
    *
    * Arguments,
    * checkpointSet, the set of checkpoints to be exported to stdout.
    *
    */
    ExportResult Export(CheckpointSet checkpointSet) noexcept;

   /*
    * Shutdown
    *
    * Shuts down the channel and cleans up resources as required.
    *
    */
    bool Shutdown();
};
```

```cpp
enum class ExportResult {
   kSuccess,
   kFailure,
};
```

## Test Strategy / Plan

Since there is a specification we will be following, we will not have to write
out user stories for testing. We will generally only be writing functional unit
tests for this project. The C++ Open Telemetry repository uses
[Googletest](https://github.com/google/googletest) because it provides test
coverage reports, also allows us to easily integrate code coverage tools such as
[codecov.io](http://codecov.io/) with the project. A required coverage target of
90% will help to ensure that our code is fully tested.

An open-source header-only testing framework called
[Catch2](https://github.com/catchorg/Catch2) is an alternate option which would
satisfy our testing needs. It is easy to use, supports behavior driven
development, and does not need to be embedded in the project as source files to
operate (unlike Googletest). Code coverage would still be possible using this
testing framework but would require us to integrate additional tools. This
framework may be preferred as an agnostic replacement for Googletest and is
widely used in open source projects.
