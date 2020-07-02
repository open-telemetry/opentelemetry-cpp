#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/common/attribute_value.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

// Enum classes to help determine instrument types in other parts of the api
enum class InstrumentKind
{
  IntCounter,
  IntUpDownCounter,
  IntValueRecorder,
  IntValueObserver,
  IntSumObserver,
  IntUpDownSumObserver,
  DoubleCounter,
  DoubleUpDownCounter,
  DoubleValueRecorder,
  DoubleValueObserver,
  DoubleSumObserver,
  DoubleUpDownSumObserver
};

// Fewer Bound types because Asynchronous instruments cannot bind
enum class BoundInstrumentKind
{
  BoundIntCounter,
  BoundIntUpDownCounter,
  BoundIntValueRecorder,
  BoundDoubleCounter,
  BoundDoubleUpDownCounter,
  BoundDoubleValueRecorder
};



class Instrument {

public:

    // Note that Instruments should be created using the Meter class.
    // Please refer to meter.h for documentation.
    Instrument() = default;


    /**
     * Base class constructor for all other instrument types.  Whether or not
     * an instrument is synchronous or bound, it requires a name, description, 
     * unit, and enabled flag.
     * 
     * @param name is the identifier of the instrumenting library
     * @param description explains what the metric captures
     * @param unit specifies the data type held in the instrument
     * @param enabled determines if the metric is currently capturing data
     * @return Instrument type with the specified attirbutes
     */
    Instrument(nostd::string_view name, 
               nostd::string_view description, 
               nostd::string_view unit, 
               bool enabled);

    // Returns true if the instrument is enabled and collecting data
    virtual bool IsEnabled() {
      // False in default implementation
      return false;
    }

    // Return the instrument name
    virtual nostd::string_view GetName(){
      return nostd::string_view("");
    }

    // Return the instrument description
    virtual nostd::string_view GetDescription(){
      return nostd::string_view("");
    }

    // Return the insrument's units of measurement
    virtual nostd::string_view GetUnits(){
      return nostd::string_view("");
    }

};

class BoundSynchronousInstrument: public Instrument {

public:

    BoundSynchronousInstrument() = default;

    BoundSynchronousInstrument(nostd::string_view name, 
                               nostd::string_view description, nostd::string_view unit, 
                               bool enabled){}

    /**
    * Frees the resources associated with this Bound Instrument.
    * The Metric from which this instrument was created is not impacted.
    *
    * @param none
    * @return void
   */
    virtual void unbind() {}

    /**
    * Records a single synchronous metric event; a call to the aggregator
    * Since this is a bound synchronous instrument, labels are not required in  * metric capture calls.
    *
    * @param value is the numerical representation of the metric being captured
    * @return void
   */
    virtual void update(nostd::variant<int,double> value) final {} //don't want this overriden because the base boundsynchronousinstrument will call the aggregator in the sdk
};


class SynchronousInstrument: public Instrument {

public:

    SynchronousInstrument() = default;

    SynchronousInstrument(nostd::string_view name, 
                          nostd::string_view description, 
                          nostd::string_view unit, bool enabled);

    /**
    * Returns a Bound Instrument associated with the specified labels.         * Multiples requests with the same set of labels may return the same
    * Bound Instrument instance.
    *
    * It is recommended that callers keep a reference to the Bound Instrument
    * instead of repeatedly calling this operation.
    *
    * @param labels the set of labels, as key-value pairs
    * @return a Bound Instrument
   */
    BoundSynchronousInstrument bind(const nostd::string_view & labels) {
      return BoundSynchronousInstrument();
    }

    /**
    * Records a single synchronous metric event. 
    * Since this is an unbound synchronous instrument, labels are required in  * metric capture calls.
    *
    * update can be used in instruments with both add or record since it simply
    * activated the aggregator
    *
    * @param labels the set of labels, as key-value pairs
    * @param value is the numerical representation of the metric being captured
    * @return void
   */
    virtual void update(nostd::variant<int,double> value, nostd::string_view &labels) final {} 
};

class ObserverResult;

class AsynchronousInstrument: public Instrument{

public:

    AsynchronousInstrument() = default;

    AsynchronousInstrument(nostd::string_view name, 
                           nostd::string_view description, 
                           nostd::string_view unit, 
                           bool enabled, 
                           void (callback)(ObserverResult)) {}

    /**
     * Captures data by activating the callback function associated with the 
     * instrument and storing its return value.  Callbacks for asychronous 
     * instruments are defined during construction.
     * 
     * @param value is the numerical representation of the metric being captured
     * @return none
     */
    virtual void observe(int value, const nostd::string_view & labels){}

    virtual void observe(double value, const nostd::string_view & labels){}

  private:

    // Callback function which takes a pointer to an Asynchronous instrument (this) type which is stored in an observer result type and returns nothing.  This function calls the instrument's observe.
    void (*callback_)(ObserverResult);
};

}
OPENTELEMETRY_END_NAMESPACE
