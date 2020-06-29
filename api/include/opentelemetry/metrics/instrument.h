#pragma once

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/common/attribute_value.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

enum class InstrumentKind
{
  Counter,
  UpDownCounter,
  ValueRecorder,
  ValueObserver,
  SumObserver,
  UpDownSumObserver
};

enum class BoundInstrumentKind
{
  BoundCounter,
  BoundUpDownCounter,
  BoundValueRecorder,
  BoundValueObserver,
  BoundSumObserver,
  BoundUpDownSumObserver
};

//Do not need the getters virtual because each base class should have access to them -- will not  be overriden.  Also, I removed all private variables and will add them in when writing the SDK because I have more flexibility with types and they are not necessary for a minimal noop implementation.
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
     * @param unit specified the data type held in the instrument
     * @param enabled determins if the metric is currently capturing data
     * @return Instrument type with the specified attirbutes
     */
    Instrument(nostd::string_view name, nostd::string_view description, nostd::string_view unit, bool enabled);

    // Returns true if the instrument is enabled and collecting data
    bool IsEnabled();

    // Return the instrument name
    nostd::string_view GetName();

    // Return the instrument description
    nostd::string_view GetDescription();

    // Return the insrument's units of measurement
    nostd::string_view GetUnits();

};


class BoundSynchronousInstrument: public Instrument {

public:

    BoundSynchronousInstrument() = default;

    BoundSynchronousInstrument(nostd::string_view name, nostd::string_view description, nostd::string_view unit, bool enabled);

    /**
    * Frees the resources associated with this Bound Instrument.
    * The Metric from which this instrument was created is not impacted.
    *
    * @param none
    * @return void
   */
    void unbind();

    /**
    * Records a single synchronous metric event.  //Call to aggregator
    * Since this is a bound synchronous instrument, labels are notrequired in  * metric capture calls.
    *
    * @param value the numerical representation of the metric being captured
    * @return void
   */
    void update(common::AttributeValue value); //don't want this virtual because the base boundsynchronousinstrument will call the aggregator in the sdk
};


// Thinking ahead, I think I want to change this to a shared pointer so I can store pointers to instruments and the meter class has access to the ref counts

class SynchronousInstrument: public Instrument {// need update virtual or not?

public:

    SynchronousInstrument() = default;

    SynchronousInstrument(nostd::string_view name, nostd::string_view description, nostd::string_view unit, bool enabled);

    /**
    * Returns a Bound Instrument associated with the specified labels.         * Multiples requests with the same set of labels may return the same
    * Bound Instrument instance.
    *
    * It is recommended that callers keep a reference to the Bound Instrument instead of always
    * calling this method for every operation.
    *
    * @param labels the set of labels, as key-value pairs.
    * @return a Bound Instrument
   */
    BoundSynchronousInstrument bind(nostd::string_view & labels);

    /**
    * Records a single synchronous metric event. 
    * Since this is an unbound synchronous instrument, labels are required in  * metric capture calls.
    *
    *
    * @param labels the set of labels, as key-value pairs.
    * @param value the numerical representation of the metric being captured
    * @return void
   */
    void update(common::AttributeValue value, nostd::string_view &labels); //add or record
};

class AsynchronousInstrument: public Instrument{

public:

    AsynchronousInstrument() = default;

    /**
     * Captures data by activating the callback function associated with the 
     * instrument and storing its return value.  Callbacks for asychronous 
     * instruments are defined during construction.
     * 
     * @param none
     * @return none
     */
    void observe();
};

}
OPENTELEMETRY_END_NAMESPACE
