OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics {
/**
 * No-op implementation of Meter. This class should not be used directly.
 */
class NoopMeter : public Meter
{
public:
  NoopMeter() = default;

  /**
   *
   * Creates a new NoopCounter<T> and returns a shared ptr to that counter.
   *
   * @param name the name of the instrument.
   * @param description a brief description of the instrument.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean that turns the metric instrument on and off.
   * @return
   */
  opentelemetry::nostd::shared_ptr<Counter<short>>  NewShortCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<Counter<short>>{ new NoopCounter<short>(name, description, unit, enabled)};
  }

  opentelemetry::nostd::shared_ptr<Counter<int>> NewIntCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<Counter<int>>{ new NoopCounter<int>(name, description, unit, enabled)};
  }

  opentelemetry::nostd::shared_ptr<Counter<float>> NewFloatCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<Counter<float>>{ new NoopCounter<float>(name, description, unit, enabled)};
  }

  opentelemetry::nostd::shared_ptr<Counter<double>> NewDoubleCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<Counter<double>>{ new NoopCounter<double>(name, description, unit, enabled)};
  }

  /**
   *
   * Creates a new NoopCounter<T> and returns a shared ptr to that counter.
   *
   * @param name the name of the instrument.
   * @param description a brief description of the instrument.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean that turns the metric instrument on and off.
   * @return
   */
  opentelemetry::nostd::shared_ptr<UpDownCounter<short>> NewShortUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<UpDownCounter<short>>{new NoopUpDownCounter<short>(name, description, unit, enabled)};
  }

  opentelemetry::nostd::shared_ptr<UpDownCounter<int>> NewIntUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<UpDownCounter<int>>{new NoopUpDownCounter<int>(name, description, unit, enabled)};
  }

  opentelemetry::nostd::shared_ptr<UpDownCounter<float>> NewFloatUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<UpDownCounter<float>>{new NoopUpDownCounter<float>(name, description, unit, enabled)};
  }

  opentelemetry::nostd::shared_ptr<UpDownCounter<double>> NewDoubleUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<UpDownCounter<double>>{new NoopUpDownCounter<double>(name, description, unit, enabled)};
  }

  /**
   *
   * Creates a new ValueRecorder<T> and returns a shared ptr to that counter.
   *
   * @param name the name of the instrument.
   * @param description a brief description of the instrument.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean that turns the metric instrument on and off.
   * @return
   */
  opentelemetry::nostd::shared_ptr<ValueRecorder<short>> NewShortValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<ValueRecorder<short>>{ new NoopValueRecorder<short>(name, description, unit, enabled)};
  }

  opentelemetry::nostd::shared_ptr<ValueRecorder<int>> NewIntValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<ValueRecorder<int>>{ new NoopValueRecorder<int>(name, description, unit, enabled)};
  }

  opentelemetry::nostd::shared_ptr<ValueRecorder<float>> NewFloatValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<ValueRecorder<float>>{ new NoopValueRecorder<float>(name, description, unit, enabled)};
  }

  opentelemetry::nostd::shared_ptr<ValueRecorder<double>> NewDoubleValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<ValueRecorder<double>>{ new NoopValueRecorder<double>(name, description, unit, enabled)};
  }

  /**
   *
   * Creates a new SumObserver<T> and returns a shared ptr to that counter.
   *
   * @param name the name of the instrument.
   * @param description a brief description of the instrument.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean that turns the metric instrument on and off.
   * @return
   */
  opentelemetry::nostd::shared_ptr<SumObserver<short>> NewShortSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<short>)) override
  {
    return nostd::shared_ptr<SumObserver<short>>{ new NoopSumObserver<short>(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::shared_ptr<SumObserver<int>> NewIntSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<int>)) override
  {
    return nostd::shared_ptr<SumObserver<int>>{ new NoopSumObserver<int>(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::shared_ptr<SumObserver<float>> NewFloatSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<float>)) override
  {
    return nostd::shared_ptr<SumObserver<float>>{ new NoopSumObserver<float>(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::shared_ptr<SumObserver<double>> NewDoubleSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<double>)) override
  {
    return nostd::shared_ptr<SumObserver<double>>{ new NoopSumObserver<double>(name, description, unit, enabled, callback)};
  }

  /**
   *
   * Creates a new UpDownSumObserver<T> and returns a shared ptr to that counter.
   *
   * @param name the name of the instrument.
   * @param description a brief description of the instrument.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean that turns the metric instrument on and off.
   * @return
   */
  opentelemetry::nostd::shared_ptr<UpDownSumObserver<short>> NewShortUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<short>)) override
  {
    return nostd::shared_ptr<UpDownSumObserver<short>>{ new NoopUpDownSumObserver<short>(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::shared_ptr<UpDownSumObserver<int>> NewIntUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<int>)) override
  {
    return nostd::shared_ptr<UpDownSumObserver<int>>{ new NoopUpDownSumObserver<int>(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::shared_ptr<UpDownSumObserver<float>> NewFloatUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<float>)) override
  {
    return nostd::shared_ptr<UpDownSumObserver<float>>{ new NoopUpDownSumObserver<float>(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::shared_ptr<UpDownSumObserver<double>> NewDoubleUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<double>)) override
  {
    return nostd::shared_ptr<UpDownSumObserver<double>>{ new NoopUpDownSumObserver<double>(name, description, unit, enabled, callback)};
  }

  /**
   *
   * Creates a new ValueObserverObserver<T> and returns a shared ptr to that counter.
   *
   * @param name the name of the instrument.
   * @param description a brief description of the instrument.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean that turns the metric instrument on and off.
   * @return
   */
  opentelemetry::nostd::shared_ptr<ValueObserver<short>> NewShortValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<short>)) override
  {
    return nostd::shared_ptr<ValueObserver<short>>{ new NoopValueObserver<short>(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::shared_ptr<ValueObserver<int>> NewIntValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<int>)) override
  {
    return nostd::shared_ptr<ValueObserver<int>>{ new NoopValueObserver<int>(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::shared_ptr<ValueObserver<float>> NewFloatValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<float>)) override
  {
    return nostd::shared_ptr<ValueObserver<float>>{ new NoopValueObserver<float>(name, description, unit, enabled, callback)};
  }

  opentelemetry::nostd::shared_ptr<ValueObserver<double>> NewDoubleValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<double>)) override
  {
    return nostd::shared_ptr<ValueObserver<double>>{ new NoopValueObserver<double>(name, description, unit, enabled, callback)};
  }

  /**
   *
   * Utility method that allows users to atomically record measurements to a set of
   * synchronous metric instruments with a common set of labels.
   *
   * @param labels the set of labels to associate with this recorder.
   * @param instrs the instruments to record to.
   * @param values the value to record to those instruments.
   */
  void RecordShortBatch(
      const trace::KeyValueIterable &labels,
      nostd::span<const nostd::shared_ptr<SynchronousInstrument<short>>> instruments,
      nostd::span<const short> values) noexcept override
  {
    // No-op
  }

  void RecordIntBatch(
      const trace::KeyValueIterable &labels,
      nostd::span<const nostd::shared_ptr<SynchronousInstrument<int>>> instruments,
      nostd::span<const int> values) noexcept override
  {
    // No-op
  }

  void RecordFloatBatch(
      const trace::KeyValueIterable &labels,
      nostd::span<const nostd::shared_ptr<SynchronousInstrument<float>>> instruments,
      nostd::span<const float> values) noexcept override
  {
    // No-op
  }

  void RecordDoubleBatch(
      const trace::KeyValueIterable &labels,
      nostd::span<const nostd::shared_ptr<SynchronousInstrument<double>>> instruments,
      nostd::span<const double> values) noexcept override
  {
    // No-op
  }
};
}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
