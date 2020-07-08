#include <gtest/gtest.h>
#include <cstring>
#include <string>
#include "opentelemetry/metrics/noop.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

void noopIntCallback(ObserverResult result)
{
  result.observe(1, "key: value");
  result.observe(-1, "key: value");
}

void noopDoubleCallback(ObserverResult result)
{
  result.observe(1.0, "key: value");
  result.observe(-1.0, "key: value");
}

TEST(ValueObserver, Observe)
{
  NoopIntValueObserver alpha("test", "none", "unitless", true, &noopIntCallback);

  NoopDoubleValueObserver beta("test", "none", "unitless", true, &noopDoubleCallback);

  alpha.observe(1, "key:value");
  beta.observe(1.0, "key:value");
}

TEST(SumObserver, DefaultConstruction)
{
  NoopIntSumObserver alpha("test", "none", "unitless", true, &noopIntCallback);

  NoopDoubleSumObserver beta("test", "none", "unitless", true, &noopDoubleCallback);

  alpha.observe(1, "key:value");
  beta.observe(1.0, "key:value");
}

TEST(UpDownSumObserver, DefaultConstruction)
{
  NoopIntUpDownSumObserver alpha("test", "none", "unitless", true, &noopIntCallback);

  NoopDoubleUpDownSumObserver beta("test", "none", "unitless", true, &noopDoubleCallback);

  alpha.observe(1, "key:value");
  beta.observe(1.0, "key:value");
  alpha.observe(-1, "key:value");
  beta.observe(-1.0, "key:value");
}

TEST(Counter, DefaultConstruction)
{
  NoopIntCounter alpha("test", "none", "unitless", true);
  NoopDoubleCounter beta("other", "none", "unitless", true);

  alpha.bind("key: value");

  auto gamma = alpha.bind("key: value");
  auto delta = beta.bind("key: value");

  gamma->unbind();
  delta->unbind();
}

TEST(Counter, Add)
{
  NoopIntCounter alpha("test", "none", "unitless", true);
  NoopDoubleCounter beta("other", "none", "unitless", true);

  alpha.add(1, "key:value");
  beta.add(1.0, "key:value");

  auto gamma = alpha.bind("key: value");
  auto delta = beta.bind("key: value");

  gamma->add(1);
  delta->add(1.0);

  gamma->unbind();
  delta->unbind();
}

TEST(UpDownCounter, DefaultConstruction)
{
  NoopIntUpDownCounter alpha("test", "none", "unitless", true);
  NoopDoubleUpDownCounter beta("other", "none", "unitless", true);

  alpha.bind("key: value");

  auto gamma = alpha.bind("key: value");
  auto delta = beta.bind("key: value");

  gamma->unbind();
  delta->unbind();
}

TEST(UpDownCounter, Add)
{
  NoopIntUpDownCounter alpha("test", "none", "unitless", true);
  NoopDoubleUpDownCounter beta("other", "none", "unitless", true);

  alpha.add(1, "key:value");
  beta.add(1.0, "key:value");

  auto gamma = alpha.bind("key: value");
  auto delta = beta.bind("key: value");

  gamma->add(1);
  delta->add(1.0);
  gamma->add(-1);
  delta->add(-1.0);

  gamma->unbind();
  delta->unbind();
}

TEST(ValueRecorder, DefaultConstruction)
{
  NoopIntValueRecorder alpha("test", "none", "unitless", true);
  NoopDoubleValueRecorder beta("other", "none", "unitless", true);

  alpha.bind("key: value");

  auto gamma = alpha.bind("key: value");
  auto delta = beta.bind("key: value");

  gamma->unbind();
  delta->unbind();
}

TEST(ValueRecorder, Record)
{
  NoopIntValueRecorder alpha("test", "none", "unitless", true);
  NoopDoubleValueRecorder beta("other", "none", "unitless", true);

  alpha.record(1, "key:value");
  beta.record(1.0, "key:value");

  auto gamma = alpha.bind("key: value");
  auto delta = beta.bind("key: value");

  gamma->record(1);
  delta->record(1.0);

  gamma->unbind();
  delta->unbind();
}

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE