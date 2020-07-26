#pragma once

#include "opentelemetry/sdk/metrics/instrument.h"
#include "opentelemetry/sdk/metrics/aggregator/counter_aggregator.h"
#include "opentelemetry/sdk/metrics/aggregator/min_max_sum_count_aggregator.h"
#include "opentelemetry/metrics/sync_instruments.h"
#include <stdexcept>
#include <map>
#include <sstream>
#include <vector>
#include <memory>

namespace metrics_api = opentelemetry::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics 
{

template <class T>
class BoundCounter final: public BoundSynchronousInstrument<T>, public metrics_api::BoundCounter<T> {

public:
    BoundCounter() = default;

    BoundCounter(nostd::string_view name,
                    nostd::string_view description,
                    nostd::string_view unit,
                    bool enabled):
                    BoundSynchronousInstrument<T>(name, description, unit, enabled,
                                                   metrics_api::InstrumentKind::Counter,nostd::shared_ptr<Aggregator<T>>(new
                                                   CounterAggregator<T>(metrics_api::InstrumentKind::Counter))) // Aggregator is chosen here
    {}

    /*
     * Add adds the value to the counter's sum. The labels are already linked to the instrument
     * and are not specified.
     *
     * @param value the numerical representation of the metric being captured
     * @param labels the set of labels, as key-value pairs
     */
    virtual void add(T value) override {
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
class Counter final : public SynchronousInstrument<T>, public metrics_api::Counter<T>
{

public:

    Counter() = default;

    Counter(nostd::string_view name,
               nostd::string_view description,
               nostd::string_view unit,
               bool enabled):
               SynchronousInstrument<T>(name, description, unit, enabled, metrics_api::InstrumentKind::Counter)
    {}

    /*
     * Bind creates a bound instrument for this counter. The labels are
     * associated with values recorded via subsequent calls to Record.
     *
     * @param labels the set of labels, as key-value pairs.
     * @return a BoundCounter tied to the specified labels
     */
    
    virtual nostd::shared_ptr<metrics_api::BoundCounter<T>> bindCounter(const trace::KeyValueIterable &labels) override {
        std::string labelset = KvToString(labels);
        if (boundInstruments_.find(labelset) == boundInstruments_.end())
        {
            auto sp1 = nostd::shared_ptr<metrics_api::BoundCounter<T>>(new BoundCounter<T>(this->name_, this->description_, this->unit_, this->enabled_));
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
    * the keys and values to be associated with this value.  Counters only
    * accept positive valued updates.
    *
    * @param value the numerical representation of the metric being captured
    * @param labels the set of labels, as key-value pairs
    */
    virtual void add(T value, const trace::KeyValueIterable &labels) override {
        this->mu_.lock();
        if (value < 0){
            throw std::invalid_argument("Counter instrument updates must be non-negative.");
        } else {
            auto sp = bindCounter(labels);
            sp->update(value);
            sp->unbind();
        }
        this->mu_.unlock();
    }

    virtual std::vector<Record> GetRecords() override {
        std::vector<Record> ret;
        for (auto x : boundInstruments_){
            auto agg_ptr = dynamic_cast<BoundCounter<T>*>(x.second.get())->GetAggregator();
            agg_ptr->checkpoint();
            ret.push_back(Record(x.second->GetName(), x.second->GetDescription(), x.first, agg_ptr));
        }
        return ret;
    }
    
    virtual void update(T value, const trace::KeyValueIterable &labels) override {
        add(value, labels);
    }

    // A collection of the bound instruments created by this unbound instrument identified by their labels.
    std::unordered_map<std::string, nostd::shared_ptr<metrics_api::BoundCounter<T>>> boundInstruments_;
};


template <class T>
class BoundUpDownCounter final: public BoundSynchronousInstrument<T>, virtual public metrics_api::BoundUpDownCounter<T> {

public:
    BoundUpDownCounter<T>() = default;

    BoundUpDownCounter<T>(nostd::string_view name,
                          nostd::string_view description,
                          nostd::string_view unit,
                          bool enabled):
                          BoundSynchronousInstrument<T>(name, description, unit, enabled,
                                                        metrics_api::InstrumentKind::UpDownCounter,
                                                        nostd::shared_ptr<Aggregator<T>>(new CounterAggregator<T>(metrics_api::InstrumentKind::UpDownCounter)))
    {}

    /*
     * Add adds the value to the counter's sum. The labels are already linked   * to the instrument
     * and are not specified.
     *
     * @param value the numerical representation of the metric being captured
     * @param labels the set of labels, as key-value pairs
     */
    virtual void add(T value) override {
        this->mu_.lock();
        this->update(value);
        this->mu_.unlock();
    }
};

template <class T>
class UpDownCounter final : public SynchronousInstrument<T>, public metrics_api::UpDownCounter<T>
{

public:
    UpDownCounter() = default;

    UpDownCounter(nostd::string_view name,
                     nostd::string_view description,
                     nostd::string_view unit,
                     bool enabled):
                     SynchronousInstrument<T>(name, description, unit, enabled, metrics_api::InstrumentKind::UpDownCounter)
    {}

    /*
     * Bind creates a bound instrument for this counter. The labels are
     * associated with values recorded via subsequent calls to Record.
     *
     * @param labels the set of labels, as key-value pairs.
     * @return a BoundIntCounter tied to the specified labels
     */
    nostd::shared_ptr<metrics_api::BoundUpDownCounter<T>> bindUpDownCounter(const trace::KeyValueIterable &labels) override
    {
        std::string labelset = KvToString(labels); // COULD CUSTOM HASH THIS INSTEAD FOR PERFORMANCE
        if (boundInstruments_.find(labelset) == boundInstruments_.end())
        {
            auto sp1 = nostd::shared_ptr<metrics_api::BoundUpDownCounter<T>>(new BoundUpDownCounter<T>(this->name_, this->description_, this->unit_, this->enabled_));
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
    void add(T value, const trace::KeyValueIterable &labels) override
    {
        this->mu_.lock();
        auto sp = bindUpDownCounter(labels);
        sp->update(value);
        sp->unbind();
        this->mu_.unlock();
    }
    
    virtual std::vector<Record> GetRecords() override {
        std::vector<Record> ret;
        for (auto x : boundInstruments_){
            auto agg_ptr = dynamic_cast<BoundCounter<T>*>(x.second.get())->GetAggregator();
            agg_ptr->checkpoint();
            ret.push_back(Record(x.second->GetName(), x.second->GetDescription(), x.first, agg_ptr));
        }
        return ret;
    }

    virtual void update(T val, const trace::KeyValueIterable &labels) override {
        add(val, labels);
    }


    std::unordered_map<std::string, nostd::shared_ptr<metrics_api::BoundUpDownCounter<T>>> boundInstruments_;
};

template <class T>
class BoundValueRecorder final: public BoundSynchronousInstrument<T> , public metrics_api::BoundValueRecorder<T>{

public:
    BoundValueRecorder() = default;

    BoundValueRecorder(nostd::string_view name,
                          nostd::string_view description,
                          nostd::string_view unit,
                          bool enabled): BoundSynchronousInstrument<T>(name, description, unit, enabled,
                                                                       metrics_api::InstrumentKind::ValueRecorder,
                                                                       nostd::shared_ptr<Aggregator<T>>(new MinMaxSumCountAggregator<T>(metrics_api::InstrumentKind::ValueRecorder))) // Aggregator is chosen here
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
        this->update(value);
        this->mu_.unlock();
    }
};

template <class T>
class ValueRecorder final : public SynchronousInstrument<T>, public metrics_api::ValueRecorder<T>
{

public:
    ValueRecorder() = default;

    ValueRecorder(nostd::string_view name,
                     nostd::string_view description,
                     nostd::string_view unit,
                     bool enabled):
                     SynchronousInstrument<T>(name, description, unit, enabled, metrics_api::InstrumentKind::ValueRecorder)
    {}

    /*
     * Bind creates a bound instrument for this counter. The labels are
     * associated with values recorded via subsequent calls to Record.
     *
     * @param labels the set of labels, as key-value pairs.
     * @return a BoundIntCounter tied to the specified labels
     */
    nostd::shared_ptr<metrics_api::BoundValueRecorder<T>> bindValueRecorder(const trace::KeyValueIterable &labels) override
    {
        std::string labelset = KvToString(labels); // COULD CUSTOM HASH THIS INSTEAD FOR PERFORMANCE
        if (boundInstruments_.find(labelset) == boundInstruments_.end())
        {
            auto sp1 = nostd::shared_ptr<metrics_api::BoundValueRecorder<T>>(new BoundValueRecorder<T>(this->name_, this->description_, this->unit_, this->enabled_));
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
    void record(T value, const trace::KeyValueIterable &labels) override
    {
        this->mu_.lock();
        auto sp = bindValueRecorder(labels);
        sp->update(value);
        sp->unbind();
        this->mu_.unlock();
    }
    
    virtual std::vector<Record> GetRecords() override {
        std::vector<Record> ret;
        for (auto x : boundInstruments_){
            auto agg_ptr = dynamic_cast<BoundCounter<T>*>(x.second.get())->GetAggregator();
            agg_ptr->checkpoint();
            ret.push_back(Record(x.second->GetName(), x.second->GetDescription(), x.first, agg_ptr));
        }
        return ret;
    }

    virtual void update(T value, const trace::KeyValueIterable &labels) override {
        record(value, labels);
    }

    std::unordered_map<std::string, nostd::shared_ptr<metrics_api::BoundValueRecorder<T>>> boundInstruments_;
};

}
}
OPENTELEMETRY_END_NAMESPACE
