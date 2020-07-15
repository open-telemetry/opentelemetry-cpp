#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/metrics/instrument.h"
#include "opentelemetry/sdk/metrics/aggregator/aggregator.h"

#include <variant>
#include <vector>
#include <mutex>
#include <memory>

namespace metrics_api = opentelemetry::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

template<class T>
class CounterAggregator : public Aggregator<T>
{
    
public:
    CounterAggregator(metrics_api::BoundInstrumentKind kind)
    {
        this->kind_ = kind;
        this->values_     = std::vector<T>(1, 0);
        this->checkpoint_ = std::vector<T>(1, 0);
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
        if (this->kind_ == other.kind_)
        {
            this->mu_.lock();
            this->values_[0] += other.values_[0];  // atomic operation afaik
            this->mu_.unlock();
        }
        else
        {
            //AggregatorMismatch Exception
        }
    }
    
    /**
     * Returns the checkpointed value
     *
     * @param none
     * @return the value of the checkpoint
     */
    std::vector<T> get_checkpoint() override
    {
        return this->checkpoint_;
    }
    
    /**
     * Returns the instrument kind which this aggregator is associated with
     *
     * @param none
     * @return the BoundInstrumentKind of the aggregator's owner
     */
    std::vector<T> get_values() override{
        return this->values_;
    }
    
};

}
}
OPENTELEMETRY_END_NAMESPACE
