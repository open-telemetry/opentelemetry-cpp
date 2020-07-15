#pragma once

#include "opentelemetry/sdk/metrics/instrument.h"
#include "opentelemetry/sdk/metrics/aggregator/counter_aggregator.h"
#include "opentelemetry/sdk/metrics/aggregator/min_max_sum_count_aggregator.h"
#include <stdexcept>
#include <map>
#include <sstream>

namespace metrics_api = opentelemetry::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics 
{

template <class T>
class BoundCounter final: public BoundSynchronousInstrument<T> {
    
public:
    BoundCounter() = default;
    
    BoundCounter(nostd::string_view name,
                    nostd::string_view description,
                    nostd::string_view unit,
                    bool enabled):
                    BoundSynchronousInstrument<T>(name, description, unit, enabled,
                                                   metrics_api::BoundInstrumentKind::BoundIntCounter,std::shared_ptr<Aggregator<T>>(new
                                                   CounterAggregator<T>(metrics_api::BoundInstrumentKind::BoundIntCounter))) // Aggregator is chosen here
    {}
    
    /*
     * Add adds the value to the counter's sum. The labels are already linked to the instrument
     * and are not specified.
     *
     * @param value the numerical representation of the metric being captured
     * @param labels the set of labels, as key-value pairs
     */
    void add(T value) {
        this->mu_.lock();
        if (value < 0){
            throw std::invalid_argument("Counter instrument updates must be non-negative.");
        } else {
            this->update(value);
        }
        this->mu_.unlock();
    }

};

template <class T>
class Counter final : public SynchronousInstrument<T>
{
    
public:
    Counter() = default;
    
    Counter(nostd::string_view name,
               nostd::string_view description,
               nostd::string_view unit,
               bool enabled):
               SynchronousInstrument<T>(name, description, unit, enabled, metrics_api::InstrumentKind::IntCounter)
    {}
    
    /*
     * Bind creates a bound instrument for this counter. The labels are
     * associated with values recorded via subsequent calls to Record.
     *
     * @param labels the set of labels, as key-value pairs.
     * @return a BoundCounter tied to the specified labels
     */
    std::shared_ptr<BoundCounter<T>> bind(const std::map<std::string, std::string> &labels)
    {
        std::string labelset = mapToString(labels);
        if (boundInstruments_.find(labelset) == boundInstruments_.end())
        {
            auto sp1 = std::make_shared<BoundCounter<T>>(this->name_, this->description_, this->unit_, this->enabled_);
            boundInstruments_[labelset]=sp1;
            return sp1;
        }
        else
        {
            boundInstruments_[labelset]->inc_ref();
            return boundInstruments_[labelset];
        }
    }
    
    /*
     * Add adds the value to the counter's sum. The labels should contain
     * the keys and values to be associated with this value.  Counters only     * accept positive
     * valued updates.
     *
     * @param value the numerical representation of the metric being captured
     * @param labels the set of labels, as key-value pairs
     */
    void add(T value, const std::map<std::string, std::string> &labels)
    {
        this->mu_.lock();
        auto sp = bind(labels);
        sp->update(value);
        sp->unbind();
        this->mu_.unlock();
    }
    
    // A collection of the bound instruments created by this unbound instrument identified by their labels.
    std::unordered_map<std::string, std::shared_ptr<BoundCounter<T>>> boundInstruments_;
};


template <class T>
class BoundUpDownCounter final: public BoundSynchronousInstrument<T> {
    
public:
    BoundUpDownCounter() = default;
    
    BoundUpDownCounter(nostd::string_view name,
                          nostd::string_view description,
                          nostd::string_view unit,
                          bool enabled):
                          BoundSynchronousInstrument<T>(name, description, unit, enabled,
                                                        metrics_api::BoundInstrumentKind::BoundIntUpDownCounter,
                                                        std::shared_ptr<Aggregator<T>>(new CounterAggregator<T>(metrics_api::BoundInstrumentKind::BoundIntUpDownCounter)))
    {}
    
    /*
     * Add adds the value to the counter's sum. The labels are already linked   * to the instrument
     * and are not specified.
     *
     * @param value the numerical representation of the metric being captured
     * @param labels the set of labels, as key-value pairs
     */
    void add(T value) {
        this->mu_.lock(); // does this need to be locked?
        this->update(value); //update calls the aggregator which is itself locked
        this->mu_.unlock(); 
    }
};

template <class T>
class UpDownCounter final : public SynchronousInstrument<T>
{
    
public:
    UpDownCounter() = default;
    
    UpDownCounter(nostd::string_view name,
                     nostd::string_view description,
                     nostd::string_view unit,
                     bool enabled):
                     SynchronousInstrument<T>(name, description, unit, enabled, metrics_api::InstrumentKind::IntUpDownCounter)
    {}
    
    /*
     * Bind creates a bound instrument for this counter. The labels are
     * associated with values recorded via subsequent calls to Record.
     *
     * @param labels the set of labels, as key-value pairs.
     * @return a BoundIntCounter tied to the specified labels
     */
    std::shared_ptr<BoundUpDownCounter<T>> bind(const std::map<std::string, std::string> &labels)
    {
        std::string labelset = mapToString(labels); // COULD CUSTOM HASH THIS INSTEAD FOR PERFORMANCE
        if (boundInstruments_.find(labelset) == boundInstruments_.end())
        {
            auto sp1 = std::make_shared<BoundUpDownCounter<T>>(this->name_, this->description_, this->unit_, this->enabled_);
            boundInstruments_[labelset]=sp1;  // perhaps use emplace
            return sp1;
        }
        else
        {
            boundInstruments_[labelset]->inc_ref();
            return boundInstruments_[labelset];
        }
    }
    
    /*
     * Add adds the value to the counter's sum. The labels should contain
     * the keys and values to be associated with this value.  Counters only     * accept positive
     * valued updates.
     *
     * @param value the numerical representation of the metric being captured
     * @param labels the set of labels, as key-value pairs
     */
    void add(T value, const std::map<std::string, std::string> &labels)
    {
        this->mu_.lock();
        auto sp = bind(labels);
        sp->update(value);
        sp->unbind();
        this->mu_.unlock();
    }
    
    std::unordered_map<std::string, std::shared_ptr<BoundUpDownCounter<T>>> boundInstruments_;
};

template <class T>
class BoundValueRecorder final: public BoundSynchronousInstrument<T> {
    
public:
    BoundValueRecorder() = default;
    
    BoundValueRecorder(nostd::string_view name,
                          nostd::string_view description,
                          nostd::string_view unit,
                          bool enabled): BoundSynchronousInstrument<T>(name, description, unit, enabled,
                                                                       metrics_api::BoundInstrumentKind::BoundIntValueRecorder,
                                                                       std::shared_ptr<Aggregator<T>>(new MinMaxSumCountAggregator<T>(metrics_api::BoundInstrumentKind::BoundIntValueRecorder))) // Aggregator is chosen here
    {}
    
    /*
     * Add adds the value to the counter's sum. The labels are already linked to the instrument
     * and are not specified.
     *
     * @param value the numerical representation of the metric being captured
     * @param labels the set of labels, as key-value pairs
     */
    void record(T value) {
        this->mu_.lock();
        update(value);
        this->mu_.unlock();
    }
};

template <class T>
class ValueRecorder final : public SynchronousInstrument<T>
{
    
public:
    ValueRecorder() = default;
    
    ValueRecorder(nostd::string_view name,
                     nostd::string_view description,
                     nostd::string_view unit,
                     bool enabled):
                     SynchronousInstrument<T>(name, description, unit, enabled, metrics_api::InstrumentKind::IntValueRecorder)
    {}
    
    /*
     * Bind creates a bound instrument for this counter. The labels are
     * associated with values recorded via subsequent calls to Record.
     *
     * @param labels the set of labels, as key-value pairs.
     * @return a BoundIntCounter tied to the specified labels
     */
    std::shared_ptr<BoundValueRecorder<T>> bind(const std::map<std::string, std::string> &labels)
    {
        std::string labelset = mapToString(labels); // COULD CUSTOM HASH THIS INSTEAD FOR PERFORMANCE
        if (boundInstruments_.find(labelset) == boundInstruments_.end())
        {
            auto sp1 = std::make_shared<BoundValueRecorder<T>>(this->name_, this->description_, this->unit_, this->enabled_);
            boundInstruments_[labelset]=sp1;  // perhaps use emplace
            return sp1;
        }
        else
        {
            boundInstruments_[labelset]->inc_ref();
            return boundInstruments_[labelset];
        }
    }
    
    /*
     * Add adds the value to the counter's sum. The labels should contain
     * the keys and values to be associated with this value.  Counters only     * accept positive
     * valued updates.
     *
     * @param value the numerical representation of the metric being captured
     * @param labels the set of labels, as key-value pairs
     */
    void record(T value, const std::map<std::string, std::string> &labels)
    {
        this->mu_.lock();
        auto sp = bind(labels);
        sp->update(value);
        sp->unbind();
        this->mu_.unlock();
    }
    
    std::unordered_map<std::string, std::shared_ptr<BoundValueRecorder<T>>> boundInstruments_;
};

}
}
OPENTELEMETRY_END_NAMESPACE
