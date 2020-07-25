#include "opentelemetry/sdk/metrics/meter.h"

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
    throw std::invalid_argument("Invalid Name");
  }
  auto counter = new Counter<short>(name, description, unit, enabled);
  auto ptr     = nostd::shared_ptr<metrics_api::Counter<short>>(counter);
  metrics_lock_.lock();
  short_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return ptr;
}

nostd::shared_ptr<metrics_api::Counter<int>> Meter::NewIntCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
    throw std::invalid_argument("Invalid Name");
  }
  auto counter = new Counter<int>(name, description, unit, enabled);
  auto ptr = nostd::shared_ptr<metrics_api::Counter<int>>(counter);
  metrics_lock_.lock();
  int_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return ptr;
}

nostd::shared_ptr<metrics_api::Counter<float>> Meter::NewFloatCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
    throw std::invalid_argument("Invalid Name");
  }
  auto counter = new Counter<float>(name, description, unit, enabled);
  auto ptr     = nostd::shared_ptr<metrics_api::Counter<float>>(counter);
  metrics_lock_.lock();
  float_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return ptr;
}

nostd::shared_ptr<metrics_api::Counter<double>> Meter::NewDoubleCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
    throw std::invalid_argument("Invalid Name");
  }
  auto counter = new Counter<double>(name, description, unit, enabled);
  auto ptr     = nostd::shared_ptr<metrics_api::Counter<double>>(counter);
  metrics_lock_.lock();
  double_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return ptr;
}

nostd::shared_ptr<metrics_api::UpDownCounter<short>> Meter::NewShortUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
    throw std::invalid_argument("Invalid Name");
  }
  auto udcounter = new UpDownCounter<short>(name, description, unit, enabled);
  auto ptr       = nostd::shared_ptr<metrics_api::UpDownCounter<short>>(udcounter);
  metrics_lock_.lock();
  short_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return ptr;
}

nostd::shared_ptr<metrics_api::UpDownCounter<int>> Meter::NewIntUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
    throw std::invalid_argument("Invalid Name");
  }
  auto udcounter = new UpDownCounter<int>(name, description, unit, enabled);
  auto ptr       = nostd::shared_ptr<metrics_api::UpDownCounter<int>>(udcounter);
  metrics_lock_.lock();
  int_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return ptr;
}

nostd::shared_ptr<metrics_api::UpDownCounter<float>> Meter::NewFloatUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
    throw std::invalid_argument("Invalid Name");
  }
  auto udcounter = new UpDownCounter<float>(name, description, unit, enabled);
  auto ptr       = nostd::shared_ptr<metrics_api::UpDownCounter<float>>(udcounter);
  metrics_lock_.lock();
  float_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return ptr;
}

nostd::shared_ptr<metrics_api::UpDownCounter<double>> Meter::NewDoubleUpDownCounter(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
    throw std::invalid_argument("Invalid Name");
  }
  auto udcounter = new UpDownCounter<double>(name, description, unit, enabled);
  auto ptr       = nostd::shared_ptr<metrics_api::UpDownCounter<double>>(udcounter);
  metrics_lock_.lock();
  double_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return ptr;
}

nostd::shared_ptr<metrics_api::ValueRecorder<short>> Meter::NewShortValueRecorder(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
    throw std::invalid_argument("Invalid Name");
  }
  auto recorder = new ValueRecorder<short>(name, description, unit, enabled);
  auto ptr      = nostd::shared_ptr<metrics_api::ValueRecorder<short>>(recorder);
  metrics_lock_.lock();
  short_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return ptr;
}

nostd::shared_ptr<metrics_api::ValueRecorder<int>> Meter::NewIntValueRecorder(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
    throw std::invalid_argument("Invalid Name");
  }
  auto recorder = new ValueRecorder<int>(name, description, unit, enabled);
  auto ptr      = nostd::shared_ptr<metrics_api::ValueRecorder<int>>(recorder);
  metrics_lock_.lock();
  int_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return ptr;
}

nostd::shared_ptr<metrics_api::ValueRecorder<float>> Meter::NewFloatValueRecorder(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
    throw std::invalid_argument("Invalid Name");
  }
  auto recorder = new ValueRecorder<float>(name, description, unit, enabled);
  auto ptr      = nostd::shared_ptr<metrics_api::ValueRecorder<float>>(recorder);
  metrics_lock_.lock();
  float_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return ptr;
}

nostd::shared_ptr<metrics_api::ValueRecorder<double>> Meter::NewDoubleValueRecorder(
    nostd::string_view name,
    nostd::string_view description,
    nostd::string_view unit,
    const bool enabled)
{
  if (!IsValidName(name) || NameAlreadyUsed(name))
  {
    throw std::invalid_argument("Invalid Name");
  }
  auto recorder = new ValueRecorder<double>(name, description, unit, enabled);
  auto ptr      = nostd::shared_ptr<metrics_api::ValueRecorder<double>>(recorder);
  metrics_lock_.lock();
  double_metrics_.insert(std::make_pair(std::string(name), ptr));
  metrics_lock_.unlock();
  return ptr;
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
    throw std::invalid_argument("Invalid Name");
  }
  auto sumobs = new SumObserver<short>(name, description, unit, enabled, callback);
  auto ptr    = nostd::shared_ptr<metrics_api::SumObserver<short>>(sumobs);
  observers_lock_.lock();
  short_observers_.insert(std::make_pair(std::string(name),ptr));
  observers_lock_.unlock();
  return ptr;
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
    throw std::invalid_argument("Invalid Name");
  }
  auto sumobs = new SumObserver<int>(name, description, unit, enabled, callback);
  auto ptr    = nostd::shared_ptr<metrics_api::SumObserver<int>>(sumobs);
  observers_lock_.lock();
  int_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return ptr;
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
    throw std::invalid_argument("Invalid Name");
  }
  auto sumobs = new SumObserver<float>(name, description, unit, enabled, callback);
  auto ptr    = nostd::shared_ptr<metrics_api::SumObserver<float>>(sumobs);
  observers_lock_.lock();
  float_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return ptr;
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
    throw std::invalid_argument("Invalid Name");
  }
  auto sumobs = new SumObserver<double>(name, description, unit, enabled, callback);
  auto ptr    = nostd::shared_ptr<metrics_api::SumObserver<double>>(sumobs);
  observers_lock_.lock();
  double_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return ptr;
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
    throw std::invalid_argument("Invalid Name");
  }
  auto sumobs = new UpDownSumObserver<short>(name, description, unit, enabled, callback);
  auto ptr    = nostd::shared_ptr<metrics_api::UpDownSumObserver<short>>(sumobs);
  observers_lock_.lock();
  short_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return ptr;
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
    throw std::invalid_argument("Invalid Name");
  }
  auto sumobs = new UpDownSumObserver<int>(name, description, unit, enabled, callback);
  auto ptr    = nostd::shared_ptr<metrics_api::UpDownSumObserver<int>>(sumobs);
  observers_lock_.lock();
  int_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return ptr;
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
    throw std::invalid_argument("Invalid Name");
  }
  auto sumobs = new UpDownSumObserver<float>(name, description, unit, enabled, callback);
  auto ptr    = nostd::shared_ptr<metrics_api::UpDownSumObserver<float>>(sumobs);
  observers_lock_.lock();
  float_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return ptr;
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
    throw std::invalid_argument("Invalid Name");
  }
  auto sumobs = new UpDownSumObserver<double>(name, description, unit, enabled, callback);
  auto ptr    = nostd::shared_ptr<metrics_api::UpDownSumObserver<double>>(sumobs);
  observers_lock_.lock();
  double_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return ptr;
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
    throw std::invalid_argument("Invalid Name");
  }
  auto sumobs = new ValueObserver<short>(name, description, unit, enabled, callback);
  auto ptr    = nostd::shared_ptr<metrics_api::ValueObserver<short>>(sumobs);
  observers_lock_.lock();
  short_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return ptr;
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
    throw std::invalid_argument("Invalid Name");
  }
  auto sumobs = new ValueObserver<int>(name, description, unit, enabled, callback);
  auto ptr    = nostd::shared_ptr<metrics_api::ValueObserver<int>>(sumobs);
  observers_lock_.lock();
  int_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return ptr;
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
    throw std::invalid_argument("Invalid Name");
  }
  auto sumobs = new ValueObserver<float>(name, description, unit, enabled, callback);
  auto ptr    = nostd::shared_ptr<metrics_api::ValueObserver<float>>(sumobs);
  observers_lock_.lock();
  float_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return ptr;
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
    throw std::invalid_argument("Invalid Name");
  }
  auto sumobs = new ValueObserver<double>(name, description, unit, enabled, callback);
  auto ptr    = nostd::shared_ptr<metrics_api::ValueObserver<double>>(sumobs);
  observers_lock_.lock();
  double_observers_.insert(std::make_pair(std::string(name), ptr));
  observers_lock_.unlock();
  return ptr;
}

void Meter::RecordShortBatch(
    const trace::KeyValueIterable &labels,
    nostd::span<const nostd::shared_ptr<metrics_api::SynchronousInstrument<short>>> instruments,
    nostd::span<const short> values) noexcept
{
  for (int i = 0; i < instruments.size(); ++i)
  {
    instruments[i]->update(values[i], labels);
  }
}

void Meter::RecordIntBatch(
    const trace::KeyValueIterable &labels,
    nostd::span<const nostd::shared_ptr<metrics_api::SynchronousInstrument<int>>> instruments,
    nostd::span<const int> values) noexcept
{
  for (int i = 0; i < instruments.size(); ++i)
  {
    instruments[i]->update(values[i], labels);
  }
}

void Meter::RecordFloatBatch(
    const trace::KeyValueIterable &labels,
    nostd::span<const nostd::shared_ptr<metrics_api::SynchronousInstrument<float>>> instruments,
    nostd::span<const float> values) noexcept
{
  for (int i = 0; i < instruments.size(); ++i)
  {
    instruments[i]->update(values[i], labels);
  }
}

void Meter::RecordDoubleBatch(
    const trace::KeyValueIterable &labels,
    nostd::span<const nostd::shared_ptr<metrics_api::SynchronousInstrument<double>>> instruments,
    nostd::span<const double> values) noexcept
{
  for (int i = 0; i < instruments.size(); ++i)
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
  for (const auto &pair : short_metrics_)
  {
    auto cast_ptr = nostd::dynamic_pointer_cast<SynchronousInstrument<short>>(pair.second);
    std::vector<Record> new_records = cast_ptr->GetRecords();
    records.insert(records.begin(), new_records.begin(), new_records.end());
  }
  for (const auto &pair : int_metrics_)
  {
    auto cast_ptr = nostd::dynamic_pointer_cast<SynchronousInstrument<int>>(pair.second);
    std::vector<Record> new_records = cast_ptr->GetRecords();
    records.insert(records.begin(), new_records.begin(), new_records.end());
  }
  for (const auto &pair : float_metrics_)
  {
    auto cast_ptr = nostd::dynamic_pointer_cast<SynchronousInstrument<float>>(pair.second);
    std::vector<Record> new_records = cast_ptr->GetRecords();
    records.insert(records.begin(), new_records.begin(), new_records.end());
  }
  for (const auto &pair : double_metrics_)
  {
    auto cast_ptr = nostd::dynamic_pointer_cast<SynchronousInstrument<double>>(pair.second);
    std::vector<Record> new_records = cast_ptr->GetRecords();
    records.insert(records.begin(), new_records.begin(), new_records.end());
  }
  metrics_lock_.unlock();
}

void Meter::CollectObservers(std::vector<Record> &records)
{
  observers_lock_.lock();
  for (const auto &pair : short_observers_)
  {
    // Must cast to sdk::SynchronousInstrument to have access to GetRecords() function
    auto cast_ptr = nostd::dynamic_pointer_cast<AsynchronousInstrument<short>>(pair.second);
    std::vector<Record> new_records = cast_ptr->GetRecords();
    records.insert(records.begin(), new_records.begin(), new_records.end());
  }
  for (const auto &pair : int_observers_)
  {
    auto cast_ptr = nostd::dynamic_pointer_cast<AsynchronousInstrument<int>>(pair.second);
    std::vector<Record> new_records = cast_ptr->GetRecords();
    records.insert(records.begin(), new_records.begin(), new_records.end());
  }
  for (const auto &pair : float_observers_)
  {
    auto cast_ptr = nostd::dynamic_pointer_cast<AsynchronousInstrument<float>>(pair.second);
    std::vector<Record> new_records = cast_ptr->GetRecords();
    records.insert(records.begin(), new_records.begin(), new_records.end());
  }
  for (const auto &pair : double_observers_)
  {
    auto cast_ptr = nostd::dynamic_pointer_cast<AsynchronousInstrument<double>>(pair.second);
    std::vector<Record> new_records = cast_ptr->GetRecords();
    records.insert(records.begin(), new_records.begin(), new_records.end());
  }
  observers_lock_.unlock();
}

bool Meter::IsValidName(nostd::string_view name)
{
  if (name.empty() || isdigit(name[0]) || isspace(name[0]) || ispunct(name[0]))
    return false;
  else
  {
    for (int i = 0; i < name.size(); ++i)
    {
      if (!isalnum(name[i]) && name[i] != '_' && name[i] != '.' && name[i] != '-')
        return false;
    }
  }
  return true;
}

bool Meter::NameAlreadyUsed(nostd::string_view name)
{
  if (short_metrics_.find(std::string(name)) != short_metrics_.end())
    return true;
  else if (int_metrics_.find(std::string(name)) != int_metrics_.end())
    return true;
  else if (float_metrics_.find(std::string(name)) != float_metrics_.end())
    return true;
  else if (double_metrics_.find(std::string(name)) != double_metrics_.end())
    return true;

  else if (short_observers_.find(std::string(name)) != short_observers_.end())
    return true;
  else if (int_observers_.find(std::string(name)) != int_observers_.end())
    return true;
  else if (float_observers_.find(std::string(name)) != float_observers_.end())
    return true;
  else if (double_observers_.find(std::string(name)) != double_observers_.end())
    return true;

  else
    return false;
}
}
}
OPENTELEMETRY_END_NAMESPACE
