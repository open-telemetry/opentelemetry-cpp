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
class HistogramAggregator : public Aggregator<T>
{
    
public:
    
    /**
     * Constructor for the histogram aggregator.  A sorted vector of boundaries is expected and boundaries are
     * doubles regardless of the aggregator's templated data type.
     *
     * Sum is stored in values_[0]
     * Count is stored in position_[1]
     */
    HistogramAggregator(metrics_api::BoundInstrumentKind kind, std::vector<double> boundaries)
    {
        this->kind_ = kind;
        boundaries_   = boundaries;
        this->values_     = std::vector<T>(2, 0);
        this->checkpoint_ = std::vector<T>(2, 0);
        bucketCounts_ = std::vector<int>(boundaries_.size() + 1, 0);
    }
    
    /**
     * Recieves a captured value from the instrument and inserts it into the current histogram counts.
     * Depending on the use case, linear search or binary search based implementations may be preferred.
     * In a uniformly distributed dataset, linear search outperforms binary search until 512 buckets.
     *
     * @param val, the raw value used in aggregation
     * @return none
     */
    void update(T val) override
    {
        
        int bucketID = boundaries_.size();
        for (int i = 0; i < boundaries_.size(); i++)
        {
            if (val < boundaries_[i]) // concurrent read is thread-safe
            {
                bucketID = i;
                break;
            }
        }
        
        // Alternate implementation with binary search
        //auto pos = std::lower_bound (boundaries_.begin(), boundaries_.end(), val);
        //bucketCounts_[pos-boundaries_.begin()] += 1;
        
        this->mu_.lock();
        this->values_[0] += val;
        this->values_[1] += 1;
        bucketCounts_[bucketID] += 1;
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
        this->values_[0]=0;
        this->values_[1]=0;
    }
    
    /**
     * Merges the values of two aggregators in a semantically accurate manner.
     * A histogram aggregator can only be merged with another histogram aggregatos with the same boudnaries.
     * A histogram merge first adds the sum and count values then iterates over the adds the bucket counts
     * element by element.
     *
     * @param other, the aggregator with merge with
     * @return none
     */
    void merge(HistogramAggregator other)
    {
        this->mu_.lock();
        this->values_[0] += other.values_[0];
        this->values_[1] += other.values_[1];
        
        for (int i = 0; i < bucketCounts_.size(); i++)
        {
            bucketCounts_[i] += other.bucketCounts_[i];
        }
        this->mu_.unlock();
    }
    
    /**
     * Returns the checkpointed value
     *
     * @param none
     * @return the value of the checkpoint
     */
    std::vector<T> get_checkpoint() override
    {
        return this->checkpoint_;  // what happens if there isn't a checkpoint?
    }
    
    /**
     * Returns the current value
     *
     * @param none
     * @return the present aggregator value
     */
    std::vector<T> get_values() override
    {
        return this->values_;
    }
    
    /**
     * Returns the bucket boundaries specified at this aggregator's creation.
     *
     * @param none
     * @return the aggregator boundaries
     */
    std::vector<double> get_boundaries()
    {
        return bucketCounts_;
    }
    
    /**
     * Returns the current counts for each bucket .
     *
     * @param none
     * @return the aggregator bucket counts
     */
    std::vector<int> get_counts()
    {
        return bucketCounts_;
    }
    
private:
    std::vector<double> boundaries_;
    std::vector<int> bucketCounts_;
};

}
}
OPENTELEMETRY_END_NAMESPACE
