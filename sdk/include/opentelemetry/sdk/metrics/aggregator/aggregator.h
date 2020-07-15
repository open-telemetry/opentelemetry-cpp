#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/metrics/instrument.h"

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
/*
 * Performs calculations necessary to combine updates from instruments into an
 * insightful value.
 * Also stores current instrument values and checkpoints collected at intervals
 * governing the entire pipeline.
 */
template<typename T>
class Aggregator
{
public:
    
    Aggregator() = default;
    
    /**
     * Recieves a captured value from the instrument and applies it to the current aggregator value.
     *
     * @param val, the raw value used in aggregation
     * @return none
     */
    virtual void update(T val) = 0;
    
    /**
     * Checkpoints the current value.  This function will overwrite the current checkpoint with the
     * current value.
     *
     * @param none
     * @return none
     */
    virtual void checkpoint() = 0;
    
    /**
     * Merges the values of two aggregators in a semantically accurate manner.
     * Merging will occur differently for different aggregators depending on the
     * way values are tracked.
     *
     * @param other, the aggregator with merge with
     * @return none
     */
    void merge(std::shared_ptr<Aggregator> other);
    
    /**
     * Returns the checkpointed value
     *
     * @param none
     * @return the value of the checkpoint
     */
    virtual std::vector<T> get_checkpoint() = 0;
    
    /**
     * Returns the current value
     *
     * @param none
     * @return the present aggregator value
     */
    virtual std::vector<T> get_values() = 0;
    
    /**
    * Returns the instrument kind which this aggregator is associated with
    *
    * @param none
    * @return the BoundInstrumentKind of the aggregator's owner
    */
    virtual opentelemetry::metrics::BoundInstrumentKind get_kind()
    {
        return kind_;
    }
    
    // Custom copy constructor to handle the mutex
    Aggregator(const Aggregator &cp) {
        values_ = cp.values_;
        checkpoint_ = cp.checkpoint_;
        kind_ = cp.kind_;
        // use default initialized mutex as they cannot be copied
    }
    
protected:
    std::vector<T> values_;
    std::vector<T> checkpoint_;
    opentelemetry::metrics::BoundInstrumentKind kind_;
    std::mutex mu_;
    
};

}  // namespace metrics
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
