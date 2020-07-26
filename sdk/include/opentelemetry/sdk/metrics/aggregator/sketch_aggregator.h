#pragma once

#include <algorithm>
#include <mutex>
#include "opentelemetry/metrics/instrument.h"
#include "opentelemetry/version.h"
#include "opentelemetry/sdk/metrics/aggregator/aggregator.h"
#include <map>
#include <stdexcept>
#include <variant>
#include <vector>

namespace metrics_api = opentelemetry::metrics;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{

/** Sketch Aggregators implement the DDSketch data type
 *  Detailed information about the algorithm can be found in the follow paper
 *  published by Datadog: http://www.vldb.org/pvldb/vol12/p2195-masson.pdf
 */

template<class T>
class SketchAggregator final : public Aggregator<T>
{
    
public:
    
    /**
     *
     *
     * Sum is stored in values_[0]
     * Count is stored in position_[1]
     */
    SketchAggregator(metrics_api::InstrumentKind kind, double error_bound, size_t max_buckets = 2048)
    {
        this->kind_ = kind;
        this->agg_kind_ = AggregatorKind::Sketch;
        this->values_   = std::vector<T>(2, 0);
        this->checkpoint_ = std::vector<T>(2, 0);
        max_buckets_ = max_buckets;
        gamma = (1+error_bound)/(1-error_bound);
    }
    
    /**
     *
     *
     * @param val, the raw value used in aggregation
     * @return none
     */
    void update(T val) override
    {
        double idx = ceil(log(val)/log(gamma));
        quantiles[idx]+=1;
        this->values_[1] += 1;
        if (quantiles.size() > max_buckets){
            int minidx = quantiles.begin()->first, minidxval = quantiles.begin()->second;
            quantiles.erase(minidx);
            quantiles[quantiles.begin()->first]+=minidxval;
        }
    }
    
    T get_quantiles (double q) override
    {
        if (q < 0 or q > 1){
            throw std::invalid_argument("Quantile values must fall between 0 and 1");
        }
        auto iter = quantiles.begin();
        
        // switch to long long for count???
        int idx = iter->first();
        int count = iter->second();
        
        // will iterator ever reach the end, think it is a possibility
        while (count < q*(this->values_[1]-1) and iter != quantiles.end()){
            iter++;
            idx = iter->first();
            count += iter->second();
        }
        return 2*pow(gamma, idx)/(gamma + 1);
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
        checkpoint_ = this->values_;
        checkpoint_quantiles_ = quantiles_;
        this->values_[0]=0;
        this->values_[1]=0;
        quantiles.clear();
    }
    
    /**
     *
     *
     * @param other, the aggregator with merge with
     * @return none
     */
    void merge(SketchAggregator other)
    {
        if (gamma != other.gamma){
            throw std::invalid_argument("Aggregators must have identical error tolerance");
        } elseif (max_buckets_ != other.max_buckets_) {
            throw std::invalid_argument("Aggregators must have the same maximum bucket allowance");
        }
        auto other_iter = other.quantiles.begin();
        while (other_iter != other.quantiles.end()){
            quantiles[other_iter->first()] += other_iter->second();
            if (quantiles.size() > max_buckets){
                int minidx = quantiles.begin()->first, minidxval = quantiles.begin()->second;
                quantiles.erase(minidx);
                quantiles[quantiles.begin()->first]+=minidxval;
            }
            other_iter++;
        }
    } // have questions about this part, mathematically does it matter if i merge after or during
    
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
     * Returns the current values
     *
     * @param none
     * @return the present aggregator values
     */
    std::vector<T> get_values() override
    {
        return this->values_;
    }
    
    
    
private:
    double gamma;
    size_t max_buckets_;
    std::map<int,int> quantiles;
    std::map<int,int> checkpoint_quantiles;
};

}
}
OPENTELEMETRY_END_NAMESPACE
