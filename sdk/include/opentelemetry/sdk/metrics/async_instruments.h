// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once
#ifdef ENABLE_METRICS_PREVIEW

#  include <map>
#  include <memory>
#  include <sstream>
#  include <stdexcept>
#  include <vector>
#  include "opentelemetry/metrics/async_instruments.h"
#  include "opentelemetry/sdk/metrics/aggregator/counter_aggregator.h"
#  include "opentelemetry/sdk/metrics/aggregator/min_max_sum_count_aggregator.h"
#  include "opentelemetry/sdk/metrics/instrument.h"
#  include "opentelemetry/version.h"

namespace metrics_api = opentelemetry::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

#  if defined(_MSC_VER)
#    pragma warning(push)
#    pragma warning(disable : 4250)  // inheriting methods via dominance
#  endif

template <class T>
class ValueObserver : public AsynchronousInstrument<T>, virtual public metrics_api::ValueObserver<T>
{

public:
  ValueObserver() = default;

  ValueObserver(nostd::string_view name,
                nostd::string_view description,
                nostd::string_view unit,
                bool enabled,
                void (*callback)(metrics_api::ObserverResult<T>))
      : AsynchronousInstrument<T>(name,
                                  description,
                                  unit,
                                  enabled,
                                  callback,
                                  metrics_api::InstrumentKind::ValueObserver)
  {}

  /*
   * Updates the instruments aggregator with the new value. The labels should
   * contain the keys and values to be associated with this value.
   *
   * @param value is the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void observe(T value, const opentelemetry::common::KeyValueIterable &labels) override
  {
    this->mu_.lock();
    std::string labelset = KvToString(labels);
    if (boundAggregators_.find(labelset) == boundAggregators_.end())
    {
      auto sp1 = std::shared_ptr<Aggregator<T>>(new MinMaxSumCountAggregator<T>(this->kind_));
      boundAggregators_.insert(std::make_pair(labelset, sp1));
      sp1->update(value);
    }
    else
    {
      boundAggregators_[labelset]->update(value);
    }
    this->mu_.unlock();
  }

  /*
   * Activate the instrument's callback function to record a measurement.  This
   * function will be called by the specified controller at a regular interval.
   *
   * @param none
   * @return none
   */
  virtual void run() override
  {
    metrics_api::ObserverResult<T> res(this);
    this->callback_(res);
  }

  virtual std::vector<Record> GetRecords() override
  {
    this->mu_.lock();
    std::vector<Record> ret;
    for (auto x : boundAggregators_)
    {
      x.second->checkpoint();
      ret.push_back(Record(this->GetName(), this->GetDescription(), x.first, x.second));
    }
    boundAggregators_.clear();
    this->mu_.unlock();
    return ret;
  }

  // Public mapping from labels (stored as strings) to their respective aggregators
  std::unordered_map<std::string, std::shared_ptr<Aggregator<T>>> boundAggregators_;
};

template <class T>
class SumObserver : public AsynchronousInstrument<T>, virtual public metrics_api::SumObserver<T>
{

public:
  SumObserver() = default;

  SumObserver(nostd::string_view name,
              nostd::string_view description,
              nostd::string_view unit,
              bool enabled,
              void (*callback)(metrics_api::ObserverResult<T>))
      : AsynchronousInstrument<T>(name,
                                  description,
                                  unit,
                                  enabled,
                                  callback,
                                  metrics_api::InstrumentKind::SumObserver)
  {}

  /*
   * Updates the instruments aggregator with the new value. The labels should
   * contain the keys and values to be associated with this value.
   *
   * @param value is the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void observe(T value, const opentelemetry::common::KeyValueIterable &labels) override
  {
    this->mu_.lock();
    std::string labelset = KvToString(labels);
    if (boundAggregators_.find(labelset) == boundAggregators_.end())
    {
      auto sp1 = std::shared_ptr<Aggregator<T>>(new CounterAggregator<T>(this->kind_));
      boundAggregators_.insert(std::make_pair(labelset, sp1));
      if (value < 0)
      {
#  if __EXCEPTIONS
        throw std::invalid_argument("Counter instrument updates must be non-negative.");
#  else
        std::terminate();
#  endif
      }
      else
      {
        sp1->update(value);
      }
    }
    else
    {
      if (value < 0)
      {
#  if __EXCEPTIONS
        throw std::invalid_argument("Counter instrument updates must be non-negative.");
#  else
        std::terminate();
#  endif
      }
      else
      {
        boundAggregators_[labelset]->update(value);
      }
    }
    this->mu_.unlock();
  }

  /*
   * Activate the intsrument's callback function to record a measurement.  This
   * function will be called by the specified controller at a regular interval.
   *
   * @param none
   * @return none
   */
  virtual void run() override
  {
    metrics_api::ObserverResult<T> res(this);
    this->callback_(res);
  }

  virtual std::vector<Record> GetRecords() override
  {
    this->mu_.lock();
    std::vector<Record> ret;
    for (auto x : boundAggregators_)
    {
      x.second->checkpoint();
      ret.push_back(Record(this->GetName(), this->GetDescription(), x.first, x.second));
    }
    boundAggregators_.clear();
    this->mu_.unlock();
    return ret;
  }

  // Public mapping from labels (stored as strings) to their respective aggregators
  std::unordered_map<std::string, std::shared_ptr<Aggregator<T>>> boundAggregators_;
};

template <class T>
class UpDownSumObserver : public AsynchronousInstrument<T>,
                          virtual public metrics_api::UpDownSumObserver<T>
{

public:
  UpDownSumObserver() = default;

  UpDownSumObserver(nostd::string_view name,
                    nostd::string_view description,
                    nostd::string_view unit,
                    bool enabled,
                    void (*callback)(metrics_api::ObserverResult<T>))
      : AsynchronousInstrument<T>(name,
                                  description,
                                  unit,
                                  enabled,
                                  callback,
                                  metrics_api::InstrumentKind::UpDownSumObserver)
  {}

  /*
   * Updates the instruments aggregator with the new value. The labels should
   * contain the keys and values to be associated with this value.
   *
   * @param value is the numerical representation of the metric being captured
   * @param labels the set of labels, as key-value pairs
   */
  virtual void observe(T value, const opentelemetry::common::KeyValueIterable &labels) override
  {
    this->mu_.lock();
    std::string labelset = KvToString(labels);
    if (boundAggregators_.find(labelset) == boundAggregators_.end())
    {
      auto sp1 = std::shared_ptr<Aggregator<T>>(new CounterAggregator<T>(this->kind_));
      boundAggregators_.insert(std::make_pair(labelset, sp1));
      sp1->update(value);
    }
    else
    {
      boundAggregators_[labelset]->update(value);
    }
    this->mu_.unlock();
  }

  /*
   * Activate the intsrument's callback function to record a measurement.  This
   * function will be called by the specified controller at a regular interval.
   *
   * @param none
   * @return none
   */
  virtual void run() override
  {
    metrics_api::ObserverResult<T> res(this);
    this->callback_(res);
  }

  virtual std::vector<Record> GetRecords() override
  {
    this->mu_.lock();
    std::vector<Record> ret;
    for (auto x : boundAggregators_)
    {
      x.second->checkpoint();
      ret.push_back(Record(this->GetName(), this->GetDescription(), x.first, x.second));
    }
    boundAggregators_.clear();
    this->mu_.unlock();
    return ret;
  }

  // Public mapping from labels (stored as strings) to their respective aggregators
  std::unordered_map<std::string, std::shared_ptr<Aggregator<T>>> boundAggregators_;
};

#  if defined(_MSC_VER)
#    pragma warning(pop)
#  endif

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
#endif
