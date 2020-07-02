#pragma once

#include "instrument.h"
#include "observer_result.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{


class IntValueObserver: public AsynchronousInstrument{

public:

    IntValueObserver() = default;

    IntValueObserver(nostd::string_view name, 
                     nostd::string_view description, 
                     nostd::string_view unit, 
                     bool enabled, 
                     void (*callback)(IntObserverResult)) {}
                     // Again, I was going to call the asynchronous base 
                     // constructor here, but do not think it is necessary

    /*
    * Updates the instruments aggregator with the new value. The labels should
    * contain the keys and values to be associated with this value.  
    * 
    * @param value is the numerical representation of the metric being captured
    * @param labels the set of labels, as key-value pairs
    */
    virtual void observe(int value, const nostd::string_view &labels) override{}

};

class DoubleValueObserver: public AsynchronousInstrument{

public:

    DoubleValueObserver() = default;

    DoubleValueObserver(nostd::string_view name, 
                        nostd::string_view description, 
                        nostd::string_view unit, 
                        bool enabled, 
                        void (*callback)(DoubleObserverResult)) {}

    virtual void observe(double value, const nostd::string_view &labels) override {}

};

class IntSumObserver: public AsynchronousInstrument{

public:

    IntSumObserver() = default;

    IntSumObserver(nostd::string_view name, 
                   nostd::string_view description, 
                   nostd::string_view unit, 
                   bool enabled, 
                   void (*callback)(IntObserverResult)) {}

    /*
    * Add adds the value to the counter's sum. The labels should contain
    * the keys and values to be associated with this value.  Counters only     * accept positive valued updates.
    * 
    * @param value is the numerical representation of the metric being captured
    * @param labels the set of labels, as key-value pairs
    */
    virtual void observe(int value, const nostd::string_view &labels) override {}

};

class DoubleSumObserver: public AsynchronousInstrument{

public:

    DoubleSumObserver() = default;

    DoubleSumObserver(nostd::string_view name, 
                      nostd::string_view description, 
                      nostd::string_view unit, 
                      bool enabled, 
                      void (*callback)(DoubleObserverResult)) {}

    virtual void observe(double value, const nostd::string_view &labels) override {}

};

class IntUpDownSumObserver: public AsynchronousInstrument{

public:

    IntUpDownSumObserver() = default;

    IntUpDownSumObserver(nostd::string_view name, 
                         nostd::string_view description, 
                         nostd::string_view unit, 
                         bool enabled, 
                         void (*callback)(IntObserverResult)) {}

    virtual void observe(int value, const nostd::string_view &labels) override {}

};

class DoubleUpDownSumObserver: public AsynchronousInstrument{

public:

    DoubleUpDownSumObserver() = default;

    DoubleUpDownSumObserver(nostd::string_view name, 
                            nostd::string_view description, 
                            nostd::string_view unit, 
                            bool enabled, 
                            void (*callback)(DoubleObserverResult)) {}

    virtual void observe(double value, const nostd::string_view &labels) override {}


};

} // namespace
OPENTELEMETRY_END_NAMESPACE