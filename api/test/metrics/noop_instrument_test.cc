// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef ENABLE_METRICS_PREVIEW
#  include <gtest/gtest.h>
#  include <cstring>
#  include <map>
#  include <string>

#  include "opentelemetry/metrics/noop.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace metrics
{

void noopIntCallback(ObserverResult<int> result)
{
  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};
  result.observe(1, labelkv);
  result.observe(-1, labelkv);
}

void noopDoubleCallback(ObserverResult<double> result)
{
  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};
  result.observe(1.5, labelkv);
  result.observe(-1.5, labelkv);
}

TEST(ValueObserver, Observe)
{
  NoopValueObserver<int> alpha("test", "none", "unitless", true, &noopIntCallback);

  NoopValueObserver<double> beta("test", "none", "unitless", true, &noopDoubleCallback);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};

  alpha.observe(1, labelkv);
  beta.observe(1.5, labelkv);
}

TEST(SumObserver, DefaultConstruction)
{
  NoopSumObserver<int> alpha("test", "none", "unitless", true, &noopIntCallback);

  NoopSumObserver<double> beta("test", "none", "unitless", true, &noopDoubleCallback);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};

  alpha.observe(1, labelkv);
  beta.observe(1.5, labelkv);
}

TEST(UpDownSumObserver, DefaultConstruction)
{
  NoopUpDownSumObserver<int> alpha("test", "none", "unitless", true, &noopIntCallback);

  NoopUpDownSumObserver<double> beta("test", "none", "unitless", true, &noopDoubleCallback);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};

  alpha.observe(1, labelkv);
  beta.observe(1.0, labelkv);
  alpha.observe(-1, labelkv);
  beta.observe(-1.0, labelkv);
}

TEST(Counter, DefaultConstruction)
{
  NoopCounter<int> alpha("test", "none", "unitless", true);
  NoopCounter<double> beta("other", "none", "unitless", true);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};

  alpha.bind(labelkv);

  auto gamma = alpha.bindNoopCounter(labelkv);
  auto delta = beta.bindNoopCounter(labelkv);

  gamma->unbind();
  delta->unbind();
}

TEST(Counter, Add)
{
  NoopCounter<int> alpha("test", "none", "unitless", true);
  NoopCounter<double> beta("other", "none", "unitless", true);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};

  alpha.add(1, labelkv);
  beta.add(1.5, labelkv);

  auto gamma = alpha.bindNoopCounter(labelkv);
  auto delta = beta.bindNoopCounter(labelkv);

  gamma->add(1);
  delta->add(1.5);

  gamma->unbind();
  delta->unbind();
}

TEST(UpDownCounter, DefaultConstruction)
{
  NoopUpDownCounter<int> alpha("test", "none", "unitless", true);
  NoopUpDownCounter<double> beta("other", "none", "unitless", true);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};

  alpha.bind(labelkv);

  auto gamma = alpha.bindNoopUpDownCounter(labelkv);
  auto delta = beta.bindNoopUpDownCounter(labelkv);

  gamma->unbind();
  delta->unbind();
}

TEST(UpDownCounter, Add)
{
  NoopUpDownCounter<int> alpha("test", "none", "unitless", true);
  NoopUpDownCounter<double> beta("other", "none", "unitless", true);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};

  alpha.add(1, labelkv);
  beta.add(1.5, labelkv);

  auto gamma = alpha.bindNoopUpDownCounter(labelkv);
  auto delta = beta.bindNoopUpDownCounter(labelkv);

  gamma->add(1);
  delta->add(1.0);
  gamma->add(-1);
  delta->add(-1.0);

  gamma->unbind();
  delta->unbind();
}

TEST(ValueRecorder, DefaultConstruction)
{
  NoopValueRecorder<int> alpha("test", "none", "unitless", true);
  NoopValueRecorder<double> beta("other", "none", "unitless", true);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};

  alpha.bind(labelkv);

  auto gamma = alpha.bindNoopValueRecorder(labelkv);
  auto delta = beta.bindNoopValueRecorder(labelkv);

  gamma->unbind();
  delta->unbind();
}

TEST(ValueRecorder, Record)
{
  NoopValueRecorder<int> alpha("test", "none", "unitless", true);
  NoopValueRecorder<double> beta("other", "none", "unitless", true);

  std::map<std::string, std::string> labels = {{"key", "value"}};
  auto labelkv = common::KeyValueIterableView<decltype(labels)>{labels};

  alpha.record(1, labelkv);
  beta.record(1.5, labelkv);

  auto gamma = alpha.bindNoopValueRecorder(labelkv);
  auto delta = beta.bindNoopValueRecorder(labelkv);

  gamma->record(1);
  delta->record(1.5);

  gamma->unbind();
  delta->unbind();
}

}  // namespace metrics
OPENTELEMETRY_END_NAMESPACE
#endif
