#pragma once

#include "opentelemetry/metrics/meter.h"
#include "opentelemetry/version.h"

#include <memory>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
namespace metrics_api = opentelemetry::metrics;
class Meter final : public metrics_api::Meter, public std::enable_shared_from_this<Meter>
{
public:
  explicit Meter(std::string library_name, std::string library_version = "")
  {
    library_name_    = library_name;
    library_version_ = library_version;
  }

  nostd::shared_ptr<metrics_api::Counter<short>> NewShortCounter(nostd::string_view name,
                                                                 nostd::string_view description,
                                                                 nostd::string_view unit,
                                                                 const bool enabled) override
  {
    return nostd::shared_ptr<metrics_api::Counter<short>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::Counter<int>> NewIntCounter(nostd::string_view name,
                                                             nostd::string_view description,
                                                             nostd::string_view unit,
                                                             const bool enabled) override
  {
    return nostd::shared_ptr<metrics_api::Counter<int>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::Counter<float>> NewFloatCounter(nostd::string_view name,
                                                                 nostd::string_view description,
                                                                 nostd::string_view unit,
                                                                 const bool enabled) override
  {
    return nostd::shared_ptr<metrics_api::Counter<float>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::Counter<double>> NewDoubleCounter(nostd::string_view name,
                                                                   nostd::string_view description,
                                                                   nostd::string_view unit,
                                                                   const bool enabled) override
  {
    return nostd::shared_ptr<metrics_api::Counter<double>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::UpDownCounter<short>> NewShortUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<metrics_api::UpDownCounter<short>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::UpDownCounter<int>> NewIntUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<metrics_api::UpDownCounter<int>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::UpDownCounter<float>> NewFloatUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<metrics_api::UpDownCounter<float>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::UpDownCounter<double>> NewDoubleUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<metrics_api::UpDownCounter<double>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::ValueRecorder<short>> NewShortValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<metrics_api::ValueRecorder<short>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::ValueRecorder<int>> NewIntValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<metrics_api::ValueRecorder<int>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::ValueRecorder<float>> NewFloatValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<metrics_api::ValueRecorder<float>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::ValueRecorder<double>> NewDoubleValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override
  {
    return nostd::shared_ptr<metrics_api::ValueRecorder<double>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::SumObserver<short>> NewShortSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(metrics_api::ObserverResult<short>)) override
  {
    return nostd::shared_ptr<metrics_api::SumObserver<short>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::SumObserver<int>> NewIntSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(metrics_api::ObserverResult<int>)) override
  {
    return nostd::shared_ptr<metrics_api::SumObserver<int>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::SumObserver<float>> NewFloatSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(metrics_api::ObserverResult<float>)) override
  {
    return nostd::shared_ptr<metrics_api::SumObserver<float>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::SumObserver<double>> NewDoubleSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(metrics_api::ObserverResult<double>)) override
  {
    return nostd::shared_ptr<metrics_api::SumObserver<double>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::UpDownSumObserver<short>> NewShortUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(metrics_api::ObserverResult<short>)) override
  {
    return nostd::shared_ptr<metrics_api::UpDownSumObserver<short>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::UpDownSumObserver<int>> NewIntUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(metrics_api::ObserverResult<int>)) override
  {
    return nostd::shared_ptr<metrics_api::UpDownSumObserver<int>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::UpDownSumObserver<float>> NewFloatUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(metrics_api::ObserverResult<float>)) override
  {
    return nostd::shared_ptr<metrics_api::UpDownSumObserver<float>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::UpDownSumObserver<double>> NewDoubleUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(metrics_api::ObserverResult<double>)) override
  {
    return nostd::shared_ptr<metrics_api::UpDownSumObserver<double>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::ValueObserver<short>> NewShortValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(metrics_api::ObserverResult<short>)) override
  {
    return nostd::shared_ptr<metrics_api::ValueObserver<short>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::ValueObserver<int>> NewIntValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(metrics_api::ObserverResult<int>)) override
  {
    return nostd::shared_ptr<metrics_api::ValueObserver<int>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::ValueObserver<float>> NewFloatValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(metrics_api::ObserverResult<float>)) override
  {
    return nostd::shared_ptr<metrics_api::ValueObserver<float>>(nullptr);
  }

  nostd::shared_ptr<metrics_api::ValueObserver<double>> NewDoubleValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(metrics_api::ObserverResult<double>)) override
  {
    return nostd::shared_ptr<metrics_api::ValueObserver<double>>(nullptr);
  }

  void RecordShortBatch(const trace::KeyValueIterable &labels,
                        nostd::span<metrics_api::SynchronousInstrument<short> *> instruments,
                        nostd::span<const short> values) noexcept override
  {}

  void RecordIntBatch(const trace::KeyValueIterable &labels,
                      nostd::span<metrics_api::SynchronousInstrument<int> *> instruments,
                      nostd::span<const int> values) noexcept override
  {}

  void RecordFloatBatch(const trace::KeyValueIterable &labels,
                        nostd::span<metrics_api::SynchronousInstrument<float> *> instruments,
                        nostd::span<const float> values) noexcept override
  {}

  void RecordDoubleBatch(const trace::KeyValueIterable &labels,
                         nostd::span<metrics_api::SynchronousInstrument<double> *> instruments,
                         nostd::span<const double> values) noexcept override
  {}

private:
  std::string library_name_;
  std::string library_version_;
};
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
