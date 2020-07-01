#pragma once

#include "instrument.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

/** 
* ObserverResult class is necessary for batch recording.  Callback functions
* to asynchronous instruments are also designed to accept a single 
* ObserverResult object and update using its pointer to the instrument itself.
*/
class ObserverResult {

public:
    ObserverResult() = default;

    ObserverResult(AsynchronousInstrument *instrument):instrument_(instrument){}

    AsynchronousInstrument *instrument_;
};

class IntObserverResult: public ObserverResult {

public:

    IntObserverResult() = default;

    IntObserverResult(AsynchronousInstrument *instrument):
                                                ObserverResult(instrument){}

    void observe(int value, const nostd::string_view &labels){
        instrument_->observe(value, labels);
    }
};


class DoubleObserverResult: public ObserverResult {

public:

    DoubleObserverResult() = default;
    
    DoubleObserverResult(AsynchronousInstrument *instrument):
                                                ObserverResult(instrument){}

    void observe(double value, const nostd::string_view &labels){
        instrument_->observe(value, labels);
    }
};

} // namespace
OPENTELEMETRY_END_NAMESPACE