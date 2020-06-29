#pragma once

#include "instrument.h"
#include "opentelemetry/common/attribute_value.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

class Result {};

class IntObserverResult: public Result {

public:

    IntObserverResult() = default;

    IntObserverResult(*AsynchronousInstrument instrument, int value);

}

class DoubleObserverResult: public Result {

public:

    DoubleObserverResult() = default;

    DoubleObserverResult(*AsynchronousInstrument instrument, double value); 

}

class IntValueObserver: public AsynchronousInstrument{

public:
    /*
    * Add adds the value to the counter's sum. The labels should contain
    * the keys and values to be associated with this value.  Counters only     * accept positive valued updates.
    * 
    * @param value the numerical representation of the metric being captured
    * @param labels the set of labels, as key-value pairs
    */
    void observe(int value, std::string_view &labels) override;

private:
    // Callback function which takes a pointer to an Asynchronous instrument (this) type which is stored in an observer result type and returns nothing.  This function calls the instrument's observe.
    void (*callback)(IntObserverResult);

}

class DoubleValueObserver: public AsynchronousInstrument{

public:
    /*
    * Add adds the value to the counter's sum. The labels should contain
    * the keys and values to be associated with this value.  Counters only     * accept positive valued updates.
    * 
    * @param value the numerical representation of the metric being captured
    * @param labels the set of labels, as key-value pairs
    */
    void observe(int value, std::string_view &labels) override;

private:
    void (*callback)(DoubleObserverResult);

}

class IntSumObserver: public AsynchronousInstrument{

public:
    /*
    * Add adds the value to the counter's sum. The labels should contain
    * the keys and values to be associated with this value.  Counters only     * accept positive valued updates.
    * 
    * @param value the numerical representation of the metric being captured
    * @param labels the set of labels, as key-value pairs
    */
    void observe(int value, std::string_view &labels) override;

private:
    void (*callback)(IntObserverResult);

}

class DoubleSumObserver: public AsynchronousInstrument{

public:
    /*
    * Add adds the value to the counter's sum. The labels should contain
    * the keys and values to be associated with this value.  Counters only     * accept positive valued updates.
    * 
    * @param value the numerical representation of the metric being captured
    * @param labels the set of labels, as key-value pairs
    */
    void observe(int value, std::string_view &labels) override;

private:
    void (*callback)(DoubleObserverResult);

}

class IntUpDownSumObserver: public AsynchronousInstrument{

public:
    /*
    * Add adds the value to the counter's sum. The labels should contain
    * the keys and values to be associated with this value.  Counters only     * accept positive valued updates.
    * 
    * @param value the numerical representation of the metric being captured
    * @param labels the set of labels, as key-value pairs
    */
    void observe(int value, std::string_view &labels) override;

private:
    void (*callback)(IntObserverResult);

}

class DoubleUpDownSumObserver: public AsynchronousInstrument{

public:
    /*
    * Add adds the value to the counter's sum. The labels should contain
    * the keys and values to be associated with this value.  Counters only     * accept positive valued updates.
    * 
    * @param value the numerical representation of the metric being captured
    * @param labels the set of labels, as key-value pairs
    */
    void observe(int value, std::string_view &labels) override;

private:
    void (*callback)(DoubleObserverResult);

}

}
OPENTELEMETRY_BEGIN_NAMESPACE