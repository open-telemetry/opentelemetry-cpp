// Copyright 2021, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef _WIN32

#  include <benchmark/benchmark.h>
#  include <gtest/gtest.h>
#  include <map>
#  include <string>
#  include <unordered_map>

#  include "opentelemetry/exporters/etw/etw_tracer_exporter.h"
#  include "opentelemetry/sdk/trace/simple_processor.h"

using namespace OPENTELEMETRY_NAMESPACE;

using Properties       = opentelemetry::exporter::ETW::Properties;
using PropertyValue    = opentelemetry::exporter::ETW::PropertyValue;
using PropertyValueMap = opentelemetry::exporter::ETW::PropertyValueMap;

namespace
{
static constexpr const char *providerName = "OpenTelemetry-ETW-StressTest";

static exporter::ETW::TracerProviderOptions providerOptions = {{"enableTraceId", false},
                                                               {"enableSpanId", false},
                                                               {"enableActivityId", false},
                                                               {"enableRelatedActivityId", false},
                                                               {"enableAutoParent", false}};

class ETWProviderStressTest
{
  exporter::ETW::TracerProvider provider_;
  std::string mode_;
  nostd::shared_ptr<trace::Tracer> tracer_;
  nostd::shared_ptr<trace::Span> span_;

public:
  /**
   * @brief Construct ETW Provider stress test object
   * @param mode Operational mode: "TLD" or "MsgPack"
   */
  ETWProviderStressTest(std::string mode = "TLD") : mode_(mode), provider_(providerOptions) {}

  /**
   * @brief Initializer tracer and start a Span
   */
  void Initialize()
  {
    tracer_ = provider_.GetTracer(providerName, mode_);
    span_   = tracer_->StartSpan("Span");
  }

  /**
   * @brief Obtain the tracer
   */
  nostd::shared_ptr<trace::Tracer> GetTracer() { return tracer_; }

  /**
   * @brief Add event using Properties container
   */
  bool AddProperties()
  {
    std::string eventName = "MyEvent";
    Properties event      = {{"uint32Key", (uint32_t)1234},
                        {"uint64Key", (uint64_t)1234567890},
                        {"strKey", "someValue"}};
    span_->AddEvent(eventName, event);
    return true;
  }

  /**
   * @brief Add event using static preallocated "reusable" Properties container.
   * This approach works well for single-threaded flows, but may not be safe in
   * some multithreaded scenarios in case if reusable `Properties` get concurrently
   * modified by different threads (writes to Properties are not thread-safe).
   */
  bool AddPropertiesStatic()
  {
    std::string eventName   = "MyEvent";
    static Properties event = {{"uint32Key", (uint32_t)1234},
                               {"uint64Key", (uint64_t)1234567890},
                               {"strKey", "someValue"}};
    span_->AddEvent(eventName, event);
    return true;
  }

  /**
   * @brief Add event using initializer list
   */
  bool AddInitList()
  {
    std::string eventName = "MyEvent";
    span_->AddEvent(eventName, {{"uint32Key", (uint32_t)1234},
                                {"uint64Key", (uint64_t)1234567890},
                                {"strKey", "someValue"}});
    return true;
  }

  /**
   * @brief Add event using unordered_map
   */
  bool AddMap()
  {
    std::string eventName                                      = "MyEvent";
    std::unordered_map<const char *, common::AttributeValue> m = {
        {"uint32Key", (uint32_t)1234},
        {"uint64Key", (uint64_t)1234567890},
        {"strKey", "someValue"}};
    span_->AddEvent(eventName, m);
    return true;
  }

  /**
   * @brief End Span and close tracer.
   */
  void Teardown()
  {
    span_->End();
    tracer_->CloseWithMicroseconds(0);
  }
};

ETWProviderStressTest provider;

/**
 * @brief Create Properties and AddEvent(Properties) to Tracer
 * @param state Benchmark state.
 */
void BM_AddPropertiesToTracer(benchmark::State &state)
{
  provider.Initialize();
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(provider.AddProperties());
  };
  provider.Teardown();
}
BENCHMARK(BM_AddPropertiesToTracer);

/**
 * @brief Create static Properties and AddEvent(Properties) to Tracer
 * @param state Benchmark state.
 */
void BM_AddPropertiesStaticToTracer(benchmark::State &state)
{
  provider.Initialize();
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(provider.AddPropertiesStatic());
  };
  provider.Teardown();
}
BENCHMARK(BM_AddPropertiesStaticToTracer);

/**
 * @brief Create event via initializer list and AddEvent({...}) to Tracer
 * @param state Benchmark state.
 */
void BM_AddInitListToTracer(benchmark::State &state)
{
  provider.Initialize();
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(provider.AddInitList());
  };
  provider.Teardown();
}
BENCHMARK(BM_AddInitListToTracer);

/**
 * @brief Create event as `std::map<std::string, common::AttributeValue>`
 * and AddEvent(event) to Tracer.
 * @param state Benchmark state.
 */
void BM_AddMapToTracer(benchmark::State &state)
{
  provider.Initialize();
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize(provider.AddMap());
  };
  provider.Teardown();
}
BENCHMARK(BM_AddMapToTracer);

}  // namespace

BENCHMARK_MAIN();

#endif