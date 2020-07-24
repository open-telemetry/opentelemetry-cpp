#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/metrics/instrument.h"
#include "opentelemetry/sdk/metrics/aggregator/aggregator.h"

#include <variant>
#include <vector>
#include <mutex>

namespace metrics_api = opentelemetry::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

template<class T>
class CounterAggregator final : public Aggregator<T>
{
    
public:
    CounterAggregator(metrics_api::InstrumentKind kind)
    {
        this->kind_ = kind;
        this->values_     = std::vector<T>(1, 0);
        this->checkpoint_ = std::vector<T>(1, 0);
        this->agg_kind_ = AggregatorKind::Counter;
    }
    
    /**
     * Recieves a captured value from the instrument and applies it to the current aggregator value.
     *
     * @param val, the raw value used in aggregation
     * @return none
     */
    void update(T val) override
    {
        this->mu_.lock();
        this->values_[0] += val;  // atomic operation
        this->mu_.unlock();
    }
    
    /**
     * Checkpoints the current value.  This function will overwrite the current checkpoint with the
     * current value.
     *
     * @param none
     * @return none
     */
    void checkpoint() override
    {
        this->checkpoint_ = this->values_;
        this->values_[0] = 0;
    }
    
    /**
     * Merges the values of two aggregators in a semantically accurate manner.
     * In this case, merging only requires the the current values of the two aggregators be summed.
     *
     * @param other, the aggregator with merge with
     * @return none
     */
    void merge(CounterAggregator other)
    {
        if (this->agg_kind_ == other.agg_kind_) {
            this->mu_.lock();
            this->values_[0] += other.values_[0];
            this->mu_.unlock();
        }
        else {
            throw std::invalid_argument("Aggregators of different types cannot be merged.");
        }
    }
    
    /**
     * Returns the checkpointed value
     *
     * @param none
     * @return the value of the checkpoint
     */
    virtual std::vector<T> get_checkpoint() override
    {
        return this->checkpoint_;
    }
    
    /**
     * Returns the current values
     *
     * @param none
     * @return the present aggregator values
     */
    virtual std::vector<T> get_values() override
    {
        return this->values_;
    }
    
};

}
}
OPENTELEMETRY_END_NAMESPACE
