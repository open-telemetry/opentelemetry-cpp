OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics {
/**
 * No-op implementation of Meter. This class should not be used directly.
 */
template <class T>
class NoopMeter : public Meter<T>
{
public:
  NoopMeter() = default;

  nostd::shared_ptr<SynchronousInstrument<T>> NewCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<SynchronousInstrument<T>>{
        new NoopCounter<T>(name, description, unit, enabled)};
  }

  nostd::shared_ptr<SynchronousInstrument<T>> NewUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<SynchronousInstrument<T>>{
        new NoopUpDownCounter<T>(name, description, unit, enabled)};
  }

  nostd::shared_ptr<SynchronousInstrument<T>> NewValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<SynchronousInstrument<T>>{
        new NoopValueRecorder<T>(name, description, unit, enabled)};
  }

  nostd::shared_ptr<AsynchronousInstrument<T>> NewSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<T>)) override
  {
    return nostd::shared_ptr<AsynchronousInstrument<T>>{
        new NoopSumObserver<T>(name, description, unit, enabled, callback)};
  }

  nostd::shared_ptr<AsynchronousInstrument<T>> NewUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<T>)) override
  {
    return nostd::shared_ptr<AsynchronousInstrument<T>>{
        new NoopUpDownSumObserver<T>(name, description, unit, enabled, callback)};
  }

  nostd::shared_ptr<AsynchronousInstrument<T>> NewValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(ObserverResult<T>)) override
  {
    return nostd::shared_ptr<AsynchronousInstrument<T>>{
        new NoopValueObserver<T>(name, description, unit, enabled, callback)};
  }
};
}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
