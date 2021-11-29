// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/_metrics/meter.h"
#  include "opentelemetry/nostd/shared_ptr.h"
#  include "opentelemetry/sdk/_metrics/async_instruments.h"
#  include "opentelemetry/sdk/_metrics/instrument.h"
#  include "opentelemetry/sdk/_metrics/record.h"
#  include "opentelemetry/sdk/_metrics/sync_instruments.h"

#  include <unordered_set>
#  include <vector>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class Meter : public opentelemetry::metrics::Meter
{
public:
  explicit Meter(std::string library_name, std::string library_version = "")
  {
    library_name_    = library_name;
    library_version_ = library_version;
  }

  /**
   * Creates a Counter with the passed characteristics and returns a shared_ptr to that Counter.
   *
   * @param name the name of the new Counter.
   * @param description a brief description of what the Counter is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created Counter.
   * @throws invalid_argument exception if name is null or does not conform to OTel syntax.
   */
  nostd::shared_ptr<opentelemetry::metrics::Counter<short>> NewShortCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override;

  nostd::shared_ptr<opentelemetry::metrics::Counter<int>> NewIntCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override;

  nostd::shared_ptr<opentelemetry::metrics::Counter<float>> NewFloatCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override;

  nostd::shared_ptr<opentelemetry::metrics::Counter<double>> NewDoubleCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override;

  /**
   * Creates an UpDownCounter with the passed characteristics and returns a shared_ptr to that
   * UpDownCounter.
   *
   * @param name the name of the new UpDownCounter.
   * @param description a brief description of what the UpDownCounter is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created UpDownCounter.
   * @throws invalid_argument exception if name is null or does not conform to OTel syntax.
   */
  nostd::shared_ptr<opentelemetry::metrics::UpDownCounter<short>> NewShortUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override;

  nostd::shared_ptr<opentelemetry::metrics::UpDownCounter<int>> NewIntUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override;

  nostd::shared_ptr<opentelemetry::metrics::UpDownCounter<float>> NewFloatUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override;

  nostd::shared_ptr<opentelemetry::metrics::UpDownCounter<double>> NewDoubleUpDownCounter(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override;

  /**
   * Creates a ValueRecorder with the passed characteristics and returns a shared_ptr to that
   * ValueRecorder.
   *
   * @param name the name of the new ValueRecorder.
   * @param description a brief description of what the ValueRecorder is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @return a shared pointer to the created DoubleValueRecorder.
   * @throws invalid_argument exception if name is null or does not conform to OTel syntax.
   */
  nostd::shared_ptr<opentelemetry::metrics::ValueRecorder<short>> NewShortValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override;

  nostd::shared_ptr<opentelemetry::metrics::ValueRecorder<int>> NewIntValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override;

  nostd::shared_ptr<opentelemetry::metrics::ValueRecorder<float>> NewFloatValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override;

  nostd::shared_ptr<opentelemetry::metrics::ValueRecorder<double>> NewDoubleValueRecorder(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled) override;

  /**
   * Creates a SumObserver with the passed characteristics and returns a shared_ptr to that
   * SumObserver.
   *
   * @param name the name of the new SumObserver.
   * @param description a brief description of what the SumObserver is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @param callback the function to be observed by the instrument.
   * @return a shared pointer to the created SumObserver.
   * @throws invalid_argument exception if name is null or does not conform to OTel syntax.
   */
  nostd::shared_ptr<opentelemetry::metrics::SumObserver<short>> NewShortSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(opentelemetry::metrics::ObserverResult<short>)) override;

  nostd::shared_ptr<opentelemetry::metrics::SumObserver<int>> NewIntSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(opentelemetry::metrics::ObserverResult<int>)) override;

  nostd::shared_ptr<opentelemetry::metrics::SumObserver<float>> NewFloatSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(opentelemetry::metrics::ObserverResult<float>)) override;

  nostd::shared_ptr<opentelemetry::metrics::SumObserver<double>> NewDoubleSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(opentelemetry::metrics::ObserverResult<double>)) override;

  /**
   * Creates an UpDownSumObserver with the passed characteristics and returns a shared_ptr to
   * that UpDowNSumObserver.
   *
   * @param name the name of the new UpDownSumObserver.
   * @param description a brief description of what the UpDownSumObserver is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @param callback the function to be observed by the instrument.
   * @return a shared pointer to the created UpDownSumObserver.
   * @throws invalid_argument exception if name is null or does not conform to OTel syntax.
   */
  nostd::shared_ptr<opentelemetry::metrics::UpDownSumObserver<short>> NewShortUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(opentelemetry::metrics::ObserverResult<short>)) override;

  nostd::shared_ptr<opentelemetry::metrics::UpDownSumObserver<int>> NewIntUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(opentelemetry::metrics::ObserverResult<int>)) override;

  nostd::shared_ptr<opentelemetry::metrics::UpDownSumObserver<float>> NewFloatUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(opentelemetry::metrics::ObserverResult<float>)) override;

  nostd::shared_ptr<opentelemetry::metrics::UpDownSumObserver<double>> NewDoubleUpDownSumObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(opentelemetry::metrics::ObserverResult<double>)) override;

  /**
   * Creates a ValueObserver with the passed characteristics and returns a shared_ptr to that
   * ValueObserver.
   *
   * @param name the name of the new ValueObserver.
   * @param description a brief description of what the ValueObserver is used for.
   * @param unit the unit of metric values following https://unitsofmeasure.org/ucum.html.
   * @param enabled a boolean value that turns on or off the metric instrument.
   * @param callback the function to be observed by the instrument.
   * @return a shared pointer to the created ValueObserver.
   * @throws invalid_argument exception if name is null or does not conform to OTel syntax.
   */
  nostd::shared_ptr<opentelemetry::metrics::ValueObserver<short>> NewShortValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(opentelemetry::metrics::ObserverResult<short>)) override;

  nostd::shared_ptr<opentelemetry::metrics::ValueObserver<int>> NewIntValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(opentelemetry::metrics::ObserverResult<int>)) override;

  nostd::shared_ptr<opentelemetry::metrics::ValueObserver<float>> NewFloatValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(opentelemetry::metrics::ObserverResult<float>)) override;

  nostd::shared_ptr<opentelemetry::metrics::ValueObserver<double>> NewDoubleValueObserver(
      nostd::string_view name,
      nostd::string_view description,
      nostd::string_view unit,
      const bool enabled,
      void (*callback)(opentelemetry::metrics::ObserverResult<double>)) override;

  /**
   * Utility method that allows users to atomically record measurements to a set of
   * synchronous metric instruments with a common set of labels.
   *
   * @param labels the set of labels to associate with this recorder.
   * @param values a span of pairs where the first element of the pair is a metric instrument
   * to record to, and the second element is the value to update that instrument with.
   */
  void RecordShortBatch(
      const opentelemetry::common::KeyValueIterable &labels,
      nostd::span<opentelemetry::metrics::SynchronousInstrument<short> *> instruments,
      nostd::span<const short> values) noexcept override;

  void RecordIntBatch(const opentelemetry::common::KeyValueIterable &labels,
                      nostd::span<opentelemetry::metrics::SynchronousInstrument<int> *> instruments,
                      nostd::span<const int> values) noexcept override;

  void RecordFloatBatch(
      const opentelemetry::common::KeyValueIterable &labels,
      nostd::span<opentelemetry::metrics::SynchronousInstrument<float> *> instruments,
      nostd::span<const float> values) noexcept override;

  void RecordDoubleBatch(
      const opentelemetry::common::KeyValueIterable &labels,
      nostd::span<opentelemetry::metrics::SynchronousInstrument<double> *> instruments,
      nostd::span<const double> values) noexcept override;

  /**
   * An SDK-only function that checkpoints the aggregators of all instruments created from
   * this meter, creates a {@code Record} out of them, and sends them for export.
   *
   * @return A vector of {@code Records} to be sent to the processor.
   */
  std::vector<Record> Collect() noexcept;

private:
  /**
   * A private function that creates records from all synchronous instruments created from
   * this meter.
   *
   * @param records A reference to the vector to push the new records to.
   */
  void CollectMetrics(std::vector<Record> &records);

  /**
   * Helper function to collect Records from a single synchronous instrument
   *
   * @tparam T The integral type of the instrument to collect from.
   * @param i A map iterator pointing to the instrument to collect from
   * @param records The vector to add the new records to.
   */
  template <typename T>
  void CollectSingleSyncInstrument(
      typename std::map<std::string,
                        std::shared_ptr<opentelemetry::metrics::SynchronousInstrument<T>>>::iterator
          i,
      std::vector<Record> &records);

  /**
   * A private function that creates records from all asynchronous instruments created from
   * this meter.
   *
   * @param records A reference to the vector to push the new records to.
   */
  void CollectObservers(std::vector<Record> &records);

  /**
   * Helper function to collect Records from a single asynchronous instrument
   *
   * @tparam T The integral type of the instrument to collect from.
   * @param i A map iterator pointing to the instrument to collect from
   * @param records The vector to add the new records to.
   */
  template <typename T>
  void CollectSingleAsyncInstrument(
      typename std::map<
          std::string,
          std::shared_ptr<opentelemetry::metrics::AsynchronousInstrument<T>>>::iterator i,
      std::vector<Record> &records);

  /**
   * Utility function  used by the meter that checks if a user-passed name abides by OpenTelemetry
   * naming rules. The rules are as follows:
   *  1. The name must not be empty.
   *  2. The name must not start with a digit, a space, or any punctuation.
   *  3. The name must only have the following chaacters:
   *      All alphanumeric characters, '.', '_' and '-'.
   *
   * @param name The name to be examined for legality.
   * @return A bool representing whether the name is valid by the OpenTelemetry syntax rules.
   */
  bool IsValidName(nostd::string_view name);

  /**
   * A utility function used by the meter to determine whether an instrument of a specified
   * name already exists in this meter.
   *
   * @param name The name to examine.
   * @return A boolean representing whether the name has already been used by this meter.
   */
  bool NameAlreadyUsed(nostd::string_view name);

  /*
   * All instruments must be stored in a map so the meter can collect on these instruments.
   * Additionally, when creating a new instrument, the meter must check if an instrument of the same
   * name already exists.
   */
  std::map<std::string, std::shared_ptr<opentelemetry::metrics::SynchronousInstrument<short>>>
      short_metrics_;
  std::map<std::string, std::shared_ptr<opentelemetry::metrics::SynchronousInstrument<int>>>
      int_metrics_;
  std::map<std::string, std::shared_ptr<opentelemetry::metrics::SynchronousInstrument<float>>>
      float_metrics_;
  std::map<std::string, std::shared_ptr<opentelemetry::metrics::SynchronousInstrument<double>>>
      double_metrics_;

  std::map<std::string, std::shared_ptr<opentelemetry::metrics::AsynchronousInstrument<short>>>
      short_observers_;
  std::map<std::string, std::shared_ptr<opentelemetry::metrics::AsynchronousInstrument<int>>>
      int_observers_;
  std::map<std::string, std::shared_ptr<opentelemetry::metrics::AsynchronousInstrument<float>>>
      float_observers_;
  std::map<std::string, std::shared_ptr<opentelemetry::metrics::AsynchronousInstrument<double>>>
      double_observers_;

  std::unordered_set<std::string> names_;

  std::string library_name_;
  std::string library_version_;

  std::mutex metrics_lock_;
  std::mutex observers_lock_;
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
