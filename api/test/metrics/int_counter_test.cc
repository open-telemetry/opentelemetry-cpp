
#include "opentelemetry/metrics/noop.h"

#include <cstring>
#include <string>

#include <gtest/gtest.h>

using namespace opentelemetry::metrics;

TEST(Counter, DefaultConstruction)
{
  NoopIntCounter alpha("test", "none", "unitless", true);
  NoopDoubleCounter beta("other", "none", "unitless", true);
  
  alpha.bind("key: value");

  BoundNoopIntCounter gamma= alpha.bind("key: value");
  BoundNoopDoubleCounter delta= beta.bind("key: value");
  
}

TEST(Counter, Add)
{
  NoopIntCounter alpha("test", "none", "unitless", true);
  NoopDoubleCounter beta("other", "none", "unitless", true);

  alpha.add(1, "key:value");
  beta.add(1.0, "key:value");

  BoundNoopIntCounter gamma= alpha.bind("key: value");
  BoundNoopDoubleCounter delta= beta.bind("key: value");

  gamma.add(1);
  delta.add(1.0);
}

TEST(UpDownCounter, DefaultConstruction)
{
  NoopIntUpDownCounter alpha("test", "none", "unitless", true);
  NoopDoubleUpDownCounter beta("other", "none", "unitless", true);
  
  alpha.bind("key: value");

  BoundNoopIntUpDownCounter gamma= alpha.bind("key: value");
  BoundNoopIntUpDownCounter delta= beta.bind("key: value");
  
}

TEST(UpDownCounter, Add)
{
  NoopIntUpDownCounter alpha("test", "none", "unitless", true);
  NoopDoubleUpDownCounter beta("other", "none", "unitless", true);

  alpha.add(1, "key:value");
  beta.add(1.0, "key:value");

  BoundNoopIntUpDownCounter gamma= alpha.bind("key: value");
  BoundNoopIntUpDownCounter delta= beta.bind("key: value");

  gamma.add(1);
  delta.add(1.0);
  gamma.add(-1);
  delta.add(-1.0);
}

TEST(ValueRecorder, DefaultConstruction)
{
  NoopIntValueRecorder alpha("test", "none", "unitless", true);
  NoopDoubleValueRecorder beta("other", "none", "unitless", true);
  
  alpha.bind("key: value");

  BoundNoopIntValueRecorder gamma= alpha.bind("key: value");
  BoundNoopDoubleValueRecorder delta= beta.bind("key: value");
  
}

TEST(ValueRecorder, Record)
{
  NoopIntValueRecorder alpha("test", "none", "unitless", true);
  NoopDoubleValueRecorder beta("other", "none", "unitless", true);

  alpha.record(1, "key:value");
  beta.record(1.0, "key:value");

  BoundNoopIntValueRecorder gamma= alpha.bind("key: value");
  BoundNoopDoubleValueRecorder delta= beta.bind("key: value");

  gamma.record(1);
  delta.record(1.0);
}