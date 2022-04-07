// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef ENABLE_METRICS_PREVIEW

#  include "opentelemetry/sdk/_metrics/meter.h"
#  include "opentelemetry/common/macros.h"

namespace metrics_api = opentelemetry::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
nostd::shared_ptr<metrics_api::Counter<short>> Meter::NewShortCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto counter = new Counter<short>(name, description, unit, enabled);
  auto ptr     = std::shared_ptr<metrics_api::Counter<short>>(counter);
  metrics_lock_.lock();
  short_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return nostd::shared_ptr<metrics_api::Counter<short>>(ptr);
}

nostd::shared_ptr<metrics_api::Counter<int>> Meter::NewIntCounter(nostd::string_view name,
                                                                  nostd::string_view description,
                                                                  nostd::string_view unit,
                                                                  const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto counter = new Counter<int>(name, description, unit, enabled);
  auto ptr     = std::shared_ptr<metrics_api::Counter<int>>(counter);
  metrics_lock_.lock();
  int_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return nostd::shared_ptr<metrics_api::Counter<int>>(ptr);
}

nostd::shared_ptr<metrics_api::Counter<float>> Meter::NewFloatCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto counter = new Counter<float>(name, description, unit, enabled);
  auto ptr     = std::shared_ptr<metrics_api::Counter<float>>(counter);
  metrics_lock_.lock();
  float_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return nostd::shared_ptr<metrics_api::Counter<float>>(ptr);
}

nostd::shared_ptr<metrics_api::Counter<double>> Meter::NewDoubleCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto counter = new Counter<double>(name, description, unit, enabled);
  auto ptr     = std::shared_ptr<metrics_api::Counter<double>>(counter);
  metrics_lock_.lock();
  double_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return nostd::shared_ptr<metrics_api::Counter<double>>(ptr);
}

nostd::shared_ptr<metrics_api::UpDownCounter<short>> Meter::NewShortUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto udcounter = new UpDownCounter<short>(name, description, unit, enabled);
  auto ptr       = std::shared_ptr<metrics_api::UpDownCounter<short>>(udcounter);
  metrics_lock_.lock();
  short_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return nostd::shared_ptr<metrics_api::UpDownCounter<short>>(ptr);
}

nostd::shared_ptr<metrics_api::UpDownCounter<int>> Meter::NewIntUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto udcounter = new UpDownCounter<int>(name, description, unit, enabled);
  auto ptr       = std::shared_ptr<metrics_api::UpDownCounter<int>>(udcounter);
  metrics_lock_.lock();
  int_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return nostd::shared_ptr<metrics_api::UpDownCounter<int>>(ptr);
}

nostd::shared_ptr<metrics_api::UpDownCounter<float>> Meter::NewFloatUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto udcounter = new UpDownCounter<float>(name, description, unit, enabled);
  auto ptr       = std::shared_ptr<metrics_api::UpDownCounter<float>>(udcounter);
  metrics_lock_.lock();
  float_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return nostd::shared_ptr<metrics_api::UpDownCounter<float>>(ptr);
}

nostd::shared_ptr<metrics_api::UpDownCounter<double>> Meter::NewDoubleUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto udcounter = new UpDownCounter<double>(name, description, unit, enabled);
  auto ptr       = std::shared_ptr<metrics_api::UpDownCounter<double>>(udcounter);
  metrics_lock_.lock();
  double_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return nostd::shared_ptr<metrics_api::UpDownCounter<double>>(ptr);
}

nostd::shared_ptr<metrics_api::ValueRecorder<short>> Meter::NewShortValueRecorder(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto recorder = new ValueRecorder<short>(name, description, unit, enabled);
  auto ptr      = std::shared_ptr<metrics_api::ValueRecorder<short>>(recorder);
  metrics_lock_.lock();
  short_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return nostd::shared_ptr<metrics_api::ValueRecorder<short>>(ptr);
}

nostd::shared_ptr<metrics_api::ValueRecorder<int>> Meter::NewIntValueRecorder(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto recorder = new ValueRecorder<int>(name, description, unit, enabled);
  auto ptr      = std::shared_ptr<metrics_api::ValueRecorder<int>>(recorder);
  metrics_lock_.lock();
  int_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return nostd::shared_ptr<metrics_api::ValueRecorder<int>>(ptr);
}

nostd::shared_ptr<metrics_api::ValueRecorder<float>> Meter::NewFloatValueRecorder(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto recorder = new ValueRecorder<float>(name, description, unit, enabled);
  auto ptr      = std::shared_ptr<metrics_api::ValueRecorder<float>>(recorder);
  metrics_lock_.lock();
  float_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return nostd::shared_ptr<metrics_api::ValueRecorder<float>>(ptr);
}

nostd::shared_ptr<metrics_api::ValueRecorder<double>> Meter::NewDoubleValueRecorder(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto recorder = new ValueRecorder<double>(name, description, unit, enabled);
  auto ptr      = std::shared_ptr<metrics_api::ValueRecorder<double>>(recorder);
  metrics_lock_.lock();
  double_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return nostd::shared_ptr<metrics_api::ValueRecorder<double>>(ptr);
}

nostd::shared_ptr<metrics_api::SumObserver<short>> Meter::NewShortSumObserver(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled,
    void (*callback)(metrics_api::ObserverResult<short>))
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto sumobs = new SumObserver<short>(name, description, unit, enabled, callback);
  auto ptr    = std::shared_ptr<metrics_api::SumObserver<short>>(sumobs);
  observers_lock_.lock();
  short_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return nostd::shared_ptr<metrics_api::SumObserver<short>>(ptr);
}

nostd::shared_ptr<metrics_api::SumObserver<int>> Meter::NewIntSumObserver(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled,
    void (*callback)(metrics_api::ObserverResult<int>))
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto sumobs = new SumObserver<int>(name, description, unit, enabled, callback);
  auto ptr    = std::shared_ptr<metrics_api::SumObserver<int>>(sumobs);
  observers_lock_.lock();
  int_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return nostd::shared_ptr<metrics_api::SumObserver<int>>(ptr);
}

nostd::shared_ptr<metrics_api::SumObserver<float>> Meter::NewFloatSumObserver(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled,
    void (*callback)(metrics_api::ObserverResult<float>))
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto sumobs = new SumObserver<float>(name, description, unit, enabled, callback);
  auto ptr    = std::shared_ptr<metrics_api::SumObserver<float>>(sumobs);
  observers_lock_.lock();
  float_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return nostd::shared_ptr<metrics_api::SumObserver<float>>(ptr);
}

nostd::shared_ptr<metrics_api::SumObserver<double>> Meter::NewDoubleSumObserver(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled,
    void (*callback)(metrics_api::ObserverResult<double>))
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto sumobs = new SumObserver<double>(name, description, unit, enabled, callback);
  auto ptr    = std::shared_ptr<metrics_api::SumObserver<double>>(sumobs);
  observers_lock_.lock();
  double_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return nostd::shared_ptr<metrics_api::SumObserver<double>>(ptr);
}

nostd::shared_ptr<metrics_api::UpDownSumObserver<short>> Meter::NewShortUpDownSumObserver(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled,
    void (*callback)(metrics_api::ObserverResult<short>))
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto sumobs = new UpDownSumObserver<short>(name, description, unit, enabled, callback);
  auto ptr    = std::shared_ptr<metrics_api::UpDownSumObserver<short>>(sumobs);
  observers_lock_.lock();
  short_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return nostd::shared_ptr<metrics_api::UpDownSumObserver<short>>(ptr);
}

nostd::shared_ptr<metrics_api::UpDownSumObserver<int>> Meter::NewIntUpDownSumObserver(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled,
    void (*callback)(metrics_api::ObserverResult<int>))
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto sumobs = new UpDownSumObserver<int>(name, description, unit, enabled, callback);
  auto ptr    = std::shared_ptr<metrics_api::UpDownSumObserver<int>>(sumobs);
  observers_lock_.lock();
  int_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return nostd::shared_ptr<metrics_api::UpDownSumObserver<int>>(ptr);
}

nostd::shared_ptr<metrics_api::UpDownSumObserver<float>> Meter::NewFloatUpDownSumObserver(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled,
    void (*callback)(metrics_api::ObserverResult<float>))
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto sumobs = new UpDownSumObserver<float>(name, description, unit, enabled, callback);
  auto ptr    = std::shared_ptr<metrics_api::UpDownSumObserver<float>>(sumobs);
  observers_lock_.lock();
  float_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return nostd::shared_ptr<metrics_api::UpDownSumObserver<float>>(ptr);
}

nostd::shared_ptr<metrics_api::UpDownSumObserver<double>> Meter::NewDoubleUpDownSumObserver(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled,
    void (*callback)(metrics_api::ObserverResult<double>))
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto sumobs = new UpDownSumObserver<double>(name, description, unit, enabled, callback);
  auto ptr    = std::shared_ptr<metrics_api::UpDownSumObserver<double>>(sumobs);
  observers_lock_.lock();
  double_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return nostd::shared_ptr<metrics_api::UpDownSumObserver<double>>(ptr);
}

nostd::shared_ptr<metrics_api::ValueObserver<short>> Meter::NewShortValueObserver(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled,
    void (*callback)(metrics_api::ObserverResult<short>))
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto sumobs = new ValueObserver<short>(name, description, unit, enabled, callback);
  auto ptr    = std::shared_ptr<metrics_api::ValueObserver<short>>(sumobs);
  observers_lock_.lock();
  short_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return nostd::shared_ptr<metrics_api::ValueObserver<short>>(ptr);
}

nostd::shared_ptr<metrics_api::ValueObserver<int>> Meter::NewIntValueObserver(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled,
    void (*callback)(metrics_api::ObserverResult<int>))
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto sumobs = new ValueObserver<int>(name, description, unit, enabled, callback);
  auto ptr    = std::shared_ptr<metrics_api::ValueObserver<int>>(sumobs);
  observers_lock_.lock();
  int_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return nostd::shared_ptr<metrics_api::ValueObserver<int>>(ptr);
}

nostd::shared_ptr<metrics_api::ValueObserver<float>> Meter::NewFloatValueObserver(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled,
    void (*callback)(metrics_api::ObserverResult<float>))
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto sumobs = new ValueObserver<float>(name, description, unit, enabled, callback);
  auto ptr    = std::shared_ptr<metrics_api::ValueObserver<float>>(sumobs);
  observers_lock_.lock();
  float_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return nostd::shared_ptr<metrics_api::ValueObserver<float>>(ptr);
}

nostd::shared_ptr<metrics_api::ValueObserver<double>> Meter::NewDoubleValueObserver(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled,
    void (*callback)(metrics_api::ObserverResult<double>))
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
#  if __EXCEPTIONS
    throw std::invalid_argument("Invalid Name");
#  else
    std::terminate();
#  endif
  }
  auto sumobs = new ValueObserver<double>(name, description, unit, enabled, callback);
  auto ptr    = std::shared_ptr<metrics_api::ValueObserver<double>>(sumobs);
  observers_lock_.lock();
  double_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return nostd::shared_ptr<metrics_api::ValueObserver<double>>(ptr);
}

void Meter::RecordShortBatch(const common::KeyValueIterable &labels,
                             nostd::span<metrics_api::SynchronousInstrument<short> *> instruments,
                             nostd::span<const short> values) noexcept
{
  for (size_t i = 0; i < instruments.size(); ++i)
  {
    instruments[i]->update(values[i], labels);
  }
}

void Meter::RecordIntBatch(const common::KeyValueIterable &labels,
                           nostd::span<metrics_api::SynchronousInstrument<int> *> instruments,
                           nostd::span<const int> values) noexcept
{
  for (size_t i = 0; i < instruments.size(); ++i)
  {
    instruments[i]->update(values[i], labels);
  }
}

void Meter::RecordFloatBatch(const common::KeyValueIterable &labels,
                             nostd::span<metrics_api::SynchronousInstrument<float> *> instruments,
                             nostd::span<const float> values) noexcept
{
  for (size_t i = 0; i < instruments.size(); ++i)
  {
    instruments[i]->update(values[i], labels);
  }
}

void Meter::RecordDoubleBatch(const common::KeyValueIterable &labels,
                              nostd::span<metrics_api::SynchronousInstrument<double> *> instruments,
                              nostd::span<const double> values) noexcept
{
  for (size_t i = 0; i < instruments.size(); ++i)
  {
    instruments[i]->update(values[i], labels);
  }
}

std::vector<Record> Meter::Collect() noexcept
{
  std::vector<Record> records;
  CollectMetrics(records);
  CollectObservers(records);
  return records;
}

// Must cast to sdk::SynchronousInstrument to have access to GetRecords() function
void Meter::CollectMetrics(std::vector<Record> &records)
{
  metrics_lock_.lock();
  for (auto i = short_metrics_.begin(); i != short_metrics_.end();)
  {
    CollectSingleSyncInstrument<short>(i, records);
    if (i->second.use_count() == 1)  // Evaluates to true if user's shared_ptr has been deleted
    {
      i = short_metrics_.erase(i);  // Remove instrument that is no longer accessible
    }
    else
    {
      i++;
    }
  }
  for (auto i = int_metrics_.begin(); i != int_metrics_.end();)
  {
    CollectSingleSyncInstrument<int>(i, records);
    if (i->second.use_count() == 1)  // Evaluates to true if user's shared_ptr has been deleted
    {
      i = int_metrics_.erase(i);  // Remove instrument that is no longer accessible
    }
    else
    {
      i++;
    }
  }
  for (auto i = float_metrics_.begin(); i != float_metrics_.end();)
  {
    CollectSingleSyncInstrument<float>(i, records);
    if (i->second.use_count() == 1)  // Evaluates to true if user's shared_ptr has been deleted
    {
      i = float_metrics_.erase(i);  // Remove instrument that is no longer accessible
    }
    else
    {
      i++;
    }
  }
  for (auto i = double_metrics_.begin(); i != double_metrics_.end();)
  {
    CollectSingleSyncInstrument<double>(i, records);
    if (i->second.use_count() == 1)  // Evaluates to true if user's shared_ptr has been deleted
    {
      i = double_metrics_.erase(i);  // Remove instrument that is no longer accessible
    }
    else
    {
      i++;
    }
  }
  metrics_lock_.unlock();
}

template <typename T>
void Meter::CollectSingleSyncInstrument(
    typename std::map<std::string, std::shared_ptr<metrics_api::SynchronousInstrument<T>>>::iterator
        i,
    std::vector<Record> &records)
{
  if (!i->second->IsEnabled())
  {
    i++;
    return;
  }
#  ifdef OPENTELEMETRY_RTTI_ENABLED
  auto cast_ptr = std::dynamic_pointer_cast<SynchronousInstrument<T>>(i->second);
#  else
  auto cast_ptr = std::static_pointer_cast<SynchronousInstrument<T>>(i->second);
#  endif
  std::vector<Record> new_records = cast_ptr->GetRecords();
  records.insert(records.begin(), new_records.begin(), new_records.end());
}

void Meter::CollectObservers(std::vector<Record> &records)
{
  observers_lock_.lock();
  for (auto i = short_observers_.begin(); i != short_observers_.end();)
  {
    CollectSingleAsyncInstrument<short>(i, records);
    if (i->second.use_count() == 1)
    {
      i = short_observers_.erase(i);
    }
    else
    {
      i++;
    }
  }
  for (auto i = int_observers_.begin(); i != int_observers_.end();)
  {
    CollectSingleAsyncInstrument<int>(i, records);
    if (i->second.use_count() == 1)
    {
      i = int_observers_.erase(i);
    }
    else
    {
      i++;
    }
  }
  for (auto i = float_observers_.begin(); i != float_observers_.end();)
  {
    CollectSingleAsyncInstrument<float>(i, records);
    if (i->second.use_count() == 1)
    {
      i = float_observers_.erase(i);
    }
    else
    {
      i++;
    }
  }
  for (auto i = double_observers_.begin(); i != double_observers_.end();)
  {
    CollectSingleAsyncInstrument<double>(i, records);
    if (i->second.use_count() == 1)
    {
      i = double_observers_.erase(i);
    }
    else
    {
      i++;
    }
  }
  observers_lock_.unlock();
}

template <typename T>
void Meter::CollectSingleAsyncInstrument(
    typename std::map<std::string,
                      std::shared_ptr<metrics_api::AsynchronousInstrument<T>>>::iterator i,
    std::vector<Record> &records)
{
  if (!i->second->IsEnabled())
  {
    i++;
    return;
  }
#  ifdef OPENTELEMETRY_RTTI_ENABLED
  auto cast_ptr = std::dynamic_pointer_cast<AsynchronousInstrument<T>>(i->second);
#  else
  auto cast_ptr = std::static_pointer_cast<AsynchronousInstrument<T>>(i->second);
#  endif
  std::vector<Record> new_records = cast_ptr->GetRecords();
  records.insert(records.begin(), new_records.begin(), new_records.end());
}

bool Meter::IsValidName(nostd::string_view name)
{
  if (name.empty() || isdigit(name[0]) || isspace(name[0]) || ispunct(name[0]))
    return false;
  else
  {
    for (size_t i = 0; i < name.size(); ++i)
    {
      if (!isalnum(name[i]) && name[i] != '_' && name[i] != '.' && name[i] != '-')
        return false;
    }
  }
  return true;
}

bool Meter::NameAlreadyUsed(nostd::string_view name)
{
  std::lock_guard<std::mutex> lg_metrics(metrics_lock_);
  std::lock_guard<std::mutex> lg_obsevers(observers_lock_);
  if (names_.find(std::string(name)) != names_.end())
    return true;
  else
  {
    names_.insert(std::string(name));
    return false;
  }
}
}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
