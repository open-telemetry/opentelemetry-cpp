#pragma once

#include "instrument.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

class ObserverResult {

public:
    ObserverResult() = default;

    ObserverResult(AsynchronousInstrument *instrument):instrument_(instrument){}

    void observe(nostd::variant<int, double> value, const nostd::string_view &labels){}

private:
    AsynchronousInstrument *instrument_;
};

class IntValueObserver: public AsynchronousInstrument{

public:

    IntValueObserver() = default;

    IntValueObserver(nostd::string_view name, nostd::string_view description, nostd::string_view unit, bool enabled, void (*callback)(ObserverResult), InstrumentKind kind): callback_(callback) {}

    /*
    * Add adds the value to the counter's sum. The labels should contain
    * the keys and values to be associated with this value.  Counters only     * accept positive valued updates.
    * 
    * @param value the numerical representation of the metric being captured
    * @param labels the set of labels, as key-value pairs
    */
    void observe(int value, const std::string_view &labels){}

private:
    // Callback function which takes a pointer to an Asynchronous instrument (this) type which is stored in an observer result type and returns nothing.  This function calls the instrument's observe.
    void (*callback_)(ObserverResult);

};

class DoubleValueObserver: public AsynchronousInstrument{

public:

    DoubleValueObserver() = default;

    DoubleValueObserver(nostd::string_view name, nostd::string_view description, nostd::string_view unit, bool enabled, void (*callback)(ObserverResult), InstrumentKind kind): callback_(callback) {}

    /*
    * Add adds the value to the counter's sum. The labels should contain
    * the keys and values to be associated with this value.  Counters only     * accept positive valued updates.
    * 
    * @param value the numerical representation of the metric being captured
    * @param labels the set of labels, as key-value pairs
    */
    void observe(double value, const std::string_view &labels){}

private:
    // Callback function which takes a pointer to an Asynchronous instrument (this) type which is stored in an observer result type and returns nothing.  This function calls the instrument's observe.
    void (*callback_)(ObserverResult);

};

class IntSumObserver: public AsynchronousInstrument{

public:

    IntSumObserver() = default;

    IntSumObserver(nostd::string_view name, nostd::string_view description, nostd::string_view unit, bool enabled, void (*callback)(ObserverResult), InstrumentKind kind): callback_(callback) {}

    /*
    * Add adds the value to the counter's sum. The labels should contain
    * the keys and values to be associated with this value.  Counters only     * accept positive valued updates.
    * 
    * @param value the numerical representation of the metric being captured
    * @param labels the set of labels, as key-value pairs
    */
    void observe(int value, const std::string_view &labels){}

private:
    // Callback function which takes a pointer to an Asynchronous instrument (this) type which is stored in an observer result type and returns nothing.  This function calls the instrument's observe.
    void (*callback_)(ObserverResult);

};

class DoubleSumObserver: public AsynchronousInstrument{

public:

    DoubleSumObserver() = default;

    DoubleSumObserver(nostd::string_view name, nostd::string_view description, nostd::string_view unit, bool enabled, void (*callback)(ObserverResult), InstrumentKind kind): callback_(callback) {}

    /*
    * Add adds the value to the counter's sum. The labels should contain
    * the keys and values to be associated with this value.  Counters only     * accept positive valued updates.
    * 
    * @param value the numerical representation of the metric being captured
    * @param labels the set of labels, as key-value pairs
    */
    void observe(double value, const std::string_view &labels){}

private:
    // Callback function which takes a pointer to an Asynchronous instrument (this) type which is stored in an observer result type and returns nothing.  This function calls the instrument's observe.
    void (*callback_)(ObserverResult);

};

class IntUpDownSumObserver: public AsynchronousInstrument{

public:

    IntUpDownSumObserver() = default;

    IntUpDownSumObserver(nostd::string_view name, nostd::string_view description, nostd::string_view unit, bool enabled, void (*callback)(ObserverResult), InstrumentKind kind): callback_(callback) {}

    /*
    * Add adds the value to the counter's sum. The labels should contain
    * the keys and values to be associated with this value.  Counters only     * accept positive valued updates.
    * 
    * @param value the numerical representation of the metric being captured
    * @param labels the set of labels, as key-value pairs
    */
    void observe(int value, const std::string_view &labels){}

private:
    // Callback function which takes a pointer to an Asynchronous instrument (this) type which is stored in an observer result type and returns nothing.  This function calls the instrument's observe.
    void (*callback_)(ObserverResult);

};

class DoubleUpDownSumObserver: public AsynchronousInstrument{

public:

    DoubleUpDownSumObserver() = default;

    DoubleUpDownSumObserver(nostd::string_view name, nostd::string_view description, nostd::string_view unit, bool enabled, void (*callback)(ObserverResult), InstrumentKind kind): callback_(callback) {}

    /*
    * Add adds the value to the counter's sum. The labels should contain
    * the keys and values to be associated with this value.  Counters only     * accept positive valued updates.
    * 
    * @param value the numerical representation of the metric being captured
    * @param labels the set of labels, as key-value pairs
    */
    void observe(double value, const std::string_view &labels){}

private:
    // Callback function which takes a pointer to an Asynchronous instrument (this) type which is stored in an observer result type and returns nothing.  This function calls the instrument's observe.
    void (*callback_)(ObserverResult);

};

} // namespace
OPENTELEMETRY_END_NAMESPACE