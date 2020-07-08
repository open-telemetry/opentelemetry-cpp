#include <gtest/gtest.h>
#include <cstring>
#include <string>
#include "opentelemetry/metrics/noop.h"
#include <map>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

void noopIntCallback(ObserverResult result)
{
  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = trace::KeyValueIterableView<decltype(labels)>{labels};
  result.observe(1, labelkv);
  result.observe(-1, labelkv);
}

void noopDoubleCallback(ObserverResult result)
{
  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = trace::KeyValueIterableView<decltype(labels)>{labels};
  result.observe(1.0, labelkv);
  result.observe(-1.0, labelkv);
}

TEST(ValueObserver, Observe)
{
  NoopIntValueObserver alpha("test", "none", "unitless", true, &noopIntCallback);

  NoopDoubleValueObserver beta("test", "none", "unitless", true, &noopDoubleCallback);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = trace::KeyValueIterableView<decltype(labels)>{labels};

  alpha.observe(1, labelkv);
  beta.observe(1.0, labelkv);
}

TEST(SumObserver, DefaultConstruction)
{
  NoopIntSumObserver alpha("test", "none", "unitless", true, &noopIntCallback);

  NoopDoubleSumObserver beta("test", "none", "unitless", true, &noopDoubleCallback);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = trace::KeyValueIterableView<decltype(labels)>{labels};

  alpha.observe(1, labelkv);
  beta.observe(1.0, labelkv);
}

TEST(UpDownSumObserver, DefaultConstruction)
{
  NoopIntUpDownSumObserver alpha("test", "none", "unitless", true, &noopIntCallback);

  NoopDoubleUpDownSumObserver beta("test", "none", "unitless", true, &noopDoubleCallback);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = trace::KeyValueIterableView<decltype(labels)>{labels};

  alpha.observe(1, labelkv);
  beta.observe(1.0, labelkv);
  alpha.observe(-1, labelkv);
  beta.observe(-1.0, labelkv);
}

TEST(Counter, DefaultConstruction)
{
  NoopIntCounter alpha("test", "none", "unitless", true);
  NoopDoubleCounter beta("other", "none", "unitless", true);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = trace::KeyValueIterableView<decltype(labels)>{labels};
  alpha.bind(labelkv);

  auto gamma = alpha.bind(labelkv);
  auto delta = beta.bind(labelkv);

  gamma->unbind();
  delta->unbind();
}

TEST(Counter, Add)
{
  NoopIntCounter alpha("test", "none", "unitless", true);
  NoopDoubleCounter beta("other", "none", "unitless", true);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = trace::KeyValueIterableView<decltype(labels)>{labels};

  alpha.add(1, labelkv);
  beta.add(1.0, labelkv);

  auto gamma = alpha.bind(labelkv);
  auto delta = beta.bind(labelkv);

  gamma->add(1);
  delta->add(1.0);

  gamma->unbind();
  delta->unbind();
}

TEST(UpDownCounter, DefaultConstruction)
{
  NoopIntUpDownCounter alpha("test", "none", "unitless", true);
  NoopDoubleUpDownCounter beta("other", "none", "unitless", true);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = trace::KeyValueIterableView<decltype(labels)>{labels};

  alpha.bind(labelkv);

  auto gamma = alpha.bind(labelkv);
  auto delta = beta.bind(labelkv);

  gamma->unbind();
  delta->unbind();
}

TEST(UpDownCounter, Add)
{
  NoopIntUpDownCounter alpha("test", "none", "unitless", true);
  NoopDoubleUpDownCounter beta("other", "none", "unitless", true);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = trace::KeyValueIterableView<decltype(labels)>{labels};

  alpha.add(1, labelkv);
  beta.add(1.0, labelkv);

  auto gamma = alpha.bind(labelkv);
  auto delta = beta.bind(labelkv);

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

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = trace::KeyValueIterableView<decltype(labels)>{labels};

  alpha.bind(labelkv);

  auto gamma = alpha.bind(labelkv);
  auto delta = beta.bind(labelkv);

  gamma->unbind();
  delta->unbind();
}

TEST(ValueRecorder, Record)
{
  NoopIntValueRecorder alpha("test", "none", "unitless", true);
  NoopDoubleValueRecorder beta("other", "none", "unitless", true);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = trace::KeyValueIterableView<decltype(labels)>{labels};

  alpha.record(1, labelkv);
  beta.record(1.0, labelkv);

  auto gamma = alpha.bind(labelkv);
  auto delta = beta.bind(labelkv);

  gamma->record(1);
  delta->record(1.0);

  gamma->unbind();
  delta->unbind();
}

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE