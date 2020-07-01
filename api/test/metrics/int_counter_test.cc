
#include "opentelemetry/metrics/noop.h"
#include <cstring>
#include <string>
#include <gtest/gtest.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

void noopIntCallback(ObserverResult result){
    result.observe(1,"key: value");
    result.observe(-1,"key: value");
}

void noopDoubleCallback(ObserverResult result){
    result.observe(1,"key: value");
    result.observe(-1,"key: value");
}

TEST(ValueObserver, Observe){
  NoopIntValueObserver alpha("test", "none", "unitless", true, &noopIntCallback, InstrumentKind::IntValueObserver);

  NoopDoubleValueObserver beta("test", "none", "unitless", true, &noopDoubleCallback, InstrumentKind::DoubleValueObserver);

  alpha.observe(1,"key:value");
  beta.observe(1.0,"key:value");
}

TEST(SumObserver, DefaultConstruction){
  NoopIntSumObserver alpha("test", "none", "unitless", true, &noopIntCallback, InstrumentKind::IntSumObserver);

  NoopDoubleSumObserver beta("test", "none", "unitless", true, &noopDoubleCallback, InstrumentKind::DoubleSumObserver);

  alpha.observe(1,"key:value");
  beta.observe(1.0,"key:value");
}

TEST(UpDownSumObserver, DefaultConstruction){
  NoopIntUpDownSumObserver alpha("test", "none", "unitless", true, &noopIntCallback, InstrumentKind::IntUpDownSumObserver);

  NoopDoubleUpDownSumObserver beta("test", "none", "unitless", true, &noopDoubleCallback, InstrumentKind::DoubleUpDownSumObserver);

  alpha.observe(1,"key:value");
  beta.observe(1.0,"key:value");
  alpha.observe(-1,"key:value");
  beta.observe(-1.0,"key:value");
}

TEST(Counter, DefaultConstruction)
{
  NoopIntCounter alpha("test", "none", "unitless", true, InstrumentKind::IntCounter);
  NoopDoubleCounter beta("other", "none", "unitless", true, InstrumentKind::DoubleCounter);
  
  alpha.bind("key: value");

  BoundNoopIntCounter gamma= alpha.bind("key: value");
  BoundNoopDoubleCounter delta= beta.bind("key: value");

  gamma.unbind();
  delta.unbind();
}

TEST(Counter, Add)
{
  NoopIntCounter alpha("test", "none", "unitless", true, InstrumentKind::IntCounter);
  NoopDoubleCounter beta("other", "none", "unitless", true, InstrumentKind::DoubleCounter);

  alpha.add(1, "key:value");
  beta.add(1.0, "key:value");

  BoundNoopIntCounter gamma= alpha.bind("key: value");
  BoundNoopDoubleCounter delta= beta.bind("key: value");

  gamma.add(1);
  delta.add(1.0);

  gamma.unbind();
  delta.unbind();
}

TEST(UpDownCounter, DefaultConstruction)
{
  NoopIntUpDownCounter alpha("test", "none", "unitless", true, InstrumentKind::IntUpDownCounter);
  NoopDoubleUpDownCounter beta("other", "none", "unitless", true, InstrumentKind::DoubleUpDownCounter);
  
  alpha.bind("key: value");

  BoundNoopIntUpDownCounter gamma= alpha.bind("key: value");
  BoundNoopIntUpDownCounter delta= beta.bind("key: value");
  
  gamma.unbind();
  delta.unbind();
}

TEST(UpDownCounter, Add)
{
  NoopIntUpDownCounter alpha("test", "none", "unitless", true, InstrumentKind::IntUpDownCounter);
  NoopDoubleUpDownCounter beta("other", "none", "unitless", true, InstrumentKind::DoubleUpDownCounter);

  alpha.add(1, "key:value");
  beta.add(1.0, "key:value");

  BoundNoopIntUpDownCounter gamma= alpha.bind("key: value");
  BoundNoopIntUpDownCounter delta= beta.bind("key: value");

  gamma.add(1);
  delta.add(1.0);
  gamma.add(-1);
  delta.add(-1.0);

  gamma.unbind();
  delta.unbind();
}

TEST(ValueRecorder, DefaultConstruction)
{
  NoopIntValueRecorder alpha("test", "none", "unitless", true, InstrumentKind::IntValueRecorder);
  NoopDoubleValueRecorder beta("other", "none", "unitless", true, InstrumentKind::DoubleValueRecorder);
  
  alpha.bind("key: value");

  BoundNoopIntValueRecorder gamma= alpha.bind("key: value");
  BoundNoopDoubleValueRecorder delta= beta.bind("key: value");
  
  gamma.unbind();
  delta.unbind();
}

TEST(ValueRecorder, Record)
{
  NoopIntValueRecorder alpha("test", "none", "unitless", true, InstrumentKind::IntValueRecorder);
  NoopDoubleValueRecorder beta("other", "none", "unitless", true, InstrumentKind::DoubleValueRecorder);

  alpha.record(1, "key:value");
  beta.record(1.0, "key:value");

  BoundNoopIntValueRecorder gamma= alpha.bind("key: value");
  BoundNoopDoubleValueRecorder delta= beta.bind("key: value");

  gamma.record(1);
  delta.record(1.0);

  gamma.unbind();
  delta.unbind();
}

}
OPENTELEMETRY_END_NAMESPACE