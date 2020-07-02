#pragma once

#include "instrument.h"
#include "opentelemetry/nostd/shared_ptr.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

/** 
* ObserverResult class is necessary for the callback recording asynchronous
* instrument use.  Callback functions asynchronous instruments are designed to
* accept a single ObserverResult object and update using its pointer to the
* instrument itself.
*/
class ObserverResult {

public:
    ObserverResult() = default;

    ObserverResult(nostd::shared_ptr<AsynchronousInstrument> instrument):instrument_(instrument){}

    nostd::shared_ptr<AsynchronousInstrument> instrument_;
};

class IntObserverResult: public ObserverResult {

public:

    IntObserverResult() = default;

    IntObserverResult(nostd::shared_ptr<AsynchronousInstrument> instrument):
                                                ObserverResult(instrument){}

    void observe(int value, const nostd::string_view &labels){
        instrument_->observe(value, labels);
    }
};


class DoubleObserverResult: public ObserverResult {

public:

    DoubleObserverResult() = default;
    
    DoubleObserverResult(nostd::shared_ptr<AsynchronousInstrument> instrument):
                                                ObserverResult(instrument){}

    void observe(double value, const nostd::string_view &labels){
        instrument_->observe(value, labels);
    }
};

} // namespace
OPENTELEMETRY_END_NAMESPACE