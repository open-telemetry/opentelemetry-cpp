// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>  // IWYU pragma: keep

#include "opentelemetry/sdk/configuration/always_off_sampler_builder.h"
#include "opentelemetry/sdk/configuration/always_on_sampler_builder.h"
#include "opentelemetry/sdk/configuration/batch_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/batch_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/composable_always_off_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_always_on_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_parent_threshold_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_probability_sampler_builder.h"
#include "opentelemetry/sdk/configuration/composable_rule_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/container_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/extension_metric_producer_builder.h"
#include "opentelemetry/sdk/configuration/extension_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/host_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_builder.h"
#include "opentelemetry/sdk/configuration/logger_configurator_builder.h"
#include "opentelemetry/sdk/configuration/meter_configurator_builder.h"
#include "opentelemetry/sdk/configuration/open_census_metric_producer_builder.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/probability_sampler_builder.h"
#include "opentelemetry/sdk/configuration/process_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/pull_metric_reader_builder.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/service_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/simple_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/tracer_configurator_builder.h"
// Complete types required: the mock builders return std::unique_ptr<T>{nullptr},
// which instantiates ~unique_ptr<T> and needs sizeof(T).
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"  // IWYU pragma: keep
#include "opentelemetry/sdk/logs/exporter.h"                            // IWYU pragma: keep
#include "opentelemetry/sdk/logs/logger_config.h"                       // IWYU pragma: keep
#include "opentelemetry/sdk/logs/processor.h"                           // IWYU pragma: keep
#include "opentelemetry/sdk/metrics/export/metric_producer.h"           // IWYU pragma: keep
#include "opentelemetry/sdk/metrics/meter_config.h"                     // IWYU pragma: keep
#include "opentelemetry/sdk/metrics/metric_reader.h"                    // IWYU pragma: keep
#include "opentelemetry/sdk/resource/resource_detector.h"               // IWYU pragma: keep
#include "opentelemetry/sdk/trace/exporter.h"                           // IWYU pragma: keep
#include "opentelemetry/sdk/trace/processor.h"                          // IWYU pragma: keep
#include "opentelemetry/sdk/trace/sampler.h"                            // IWYU pragma: keep
#include "opentelemetry/sdk/trace/samplers/composable_sampler.h"        // IWYU pragma: keep
#include "opentelemetry/sdk/trace/tracer_config.h"                      // IWYU pragma: keep

namespace configuration = opentelemetry::sdk::configuration;

namespace
{

class TestAlwaysOnSamplerBuilder : public configuration::AlwaysOnSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const configuration::AlwaysOnSamplerConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestAlwaysOffSamplerBuilder : public configuration::AlwaysOffSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const configuration::AlwaysOffSamplerConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestTraceIdRatioBasedSamplerBuilder : public configuration::TraceIdRatioBasedSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const configuration::TraceIdRatioBasedSamplerConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestProbabilitySamplerBuilder : public configuration::ProbabilitySamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const configuration::ProbabilitySamplerConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestParentBasedSamplerBuilder : public configuration::ParentBasedSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const configuration::ParentBasedSamplerConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> && /* root */,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> && /* remote_parent_sampled */,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> && /* remote_parent_not_sampled */,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> && /* local_parent_sampled */,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> && /* local_parent_not_sampled */)
      const override
  {
    return nullptr;
  }
};

class TestJaegerRemoteSamplerBuilder : public configuration::JaegerRemoteSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const configuration::JaegerRemoteSamplerConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> && /* initial_sampler */) const override
  {
    return nullptr;
  }
};

class TestComposableAlwaysOnSamplerBuilder : public configuration::ComposableAlwaysOnSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> Build(
      const configuration::ComposableAlwaysOnSamplerConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestComposableAlwaysOffSamplerBuilder
    : public configuration::ComposableAlwaysOffSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> Build(
      const configuration::ComposableAlwaysOffSamplerConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestComposableProbabilitySamplerBuilder
    : public configuration::ComposableProbabilitySamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> Build(
      const configuration::ComposableProbabilitySamplerConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestComposableParentThresholdSamplerBuilder
    : public configuration::ComposableParentThresholdSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> Build(
      const configuration::ComposableParentThresholdSamplerConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> && /* root */) const override
  {
    return nullptr;
  }
};

class TestComposableRuleBasedSamplerBuilder
    : public configuration::ComposableRuleBasedSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> Build(
      const configuration::ComposableRuleBasedSamplerConfiguration * /* model */,
      std::vector<std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler>>
          && /* rule_samplers */) const override
  {
    return nullptr;
  }
};

class TestBatchSpanProcessorBuilder : public configuration::BatchSpanProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> Build(
      const configuration::BatchSpanProcessorConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> && /* exporter */) const override
  {
    return nullptr;
  }
};

class TestSimpleSpanProcessorBuilder : public configuration::SimpleSpanProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> Build(
      const configuration::SimpleSpanProcessorConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> && /* exporter */) const override
  {
    return nullptr;
  }
};

class TestBatchLogRecordProcessorBuilder : public configuration::BatchLogRecordProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> Build(
      const configuration::BatchLogRecordProcessorConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> && /* exporter */) const override
  {
    return nullptr;
  }
};

class TestSimpleLogRecordProcessorBuilder : public configuration::SimpleLogRecordProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> Build(
      const configuration::SimpleLogRecordProcessorConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> && /* exporter */) const override
  {
    return nullptr;
  }
};

class TestPullMetricReaderBuilder : public configuration::PullMetricReaderBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> Build(
      const configuration::PullMetricReaderConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> && /* exporter */) const override
  {
    return nullptr;
  }
};

class TestOpenCensusMetricProducerBuilder : public configuration::OpenCensusMetricProducerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::MetricProducer> Build(
      const configuration::OpenCensusMetricProducerConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestExtensionMetricProducerBuilder : public configuration::ExtensionMetricProducerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::MetricProducer> Build(
      const configuration::ExtensionMetricProducerConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestContainerResourceDetectorBuilder : public configuration::ContainerResourceDetectorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> Build(
      const configuration::ContainerResourceDetectorConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestHostResourceDetectorBuilder : public configuration::HostResourceDetectorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> Build(
      const configuration::HostResourceDetectorConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestProcessResourceDetectorBuilder : public configuration::ProcessResourceDetectorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> Build(
      const configuration::ProcessResourceDetectorConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestServiceResourceDetectorBuilder : public configuration::ServiceResourceDetectorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> Build(
      const configuration::ServiceResourceDetectorConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestExtensionResourceDetectorBuilder : public configuration::ExtensionResourceDetectorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::resource::ResourceDetector> Build(
      const configuration::ExtensionResourceDetectorConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestTracerConfiguratorBuilder : public configuration::TracerConfiguratorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::instrumentationscope::ScopeConfigurator<
      opentelemetry::sdk::trace::TracerConfig>>
  Build(const configuration::TracerConfiguratorConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestMeterConfiguratorBuilder : public configuration::MeterConfiguratorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::instrumentationscope::ScopeConfigurator<
      opentelemetry::sdk::metrics::MeterConfig>>
  Build(const configuration::MeterConfiguratorConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestLoggerConfiguratorBuilder : public configuration::LoggerConfiguratorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::instrumentationscope::ScopeConfigurator<
      opentelemetry::sdk::logs::LoggerConfig>>
  Build(const configuration::LoggerConfiguratorConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

}  // namespace

// Exercises the empty → set → replace lifecycle for a typed (non-named-map) registry slot.
template <typename MockType, typename GetBuilderFn, typename SetBuilderFn>
void TestTypedSlot(GetBuilderFn getter, SetBuilderFn setter)
{
  configuration::Registry registry;
  ASSERT_EQ(std::invoke(getter, registry), nullptr);

  auto first            = std::make_unique<MockType>();
  const auto *first_ptr = first.get();
  std::invoke(setter, registry, std::move(first));
  ASSERT_EQ(std::invoke(getter, registry), first_ptr);

  auto second            = std::make_unique<MockType>();
  const auto *second_ptr = second.get();
  std::invoke(setter, registry, std::move(second));
  ASSERT_EQ(std::invoke(getter, registry), second_ptr);
}

// Exercises the empty → set → replace lifecycle for a named-map registry slot.
template <typename MockType, typename GetBuilderFn, typename SetBuilderFn>
void TestNamedSlot(GetBuilderFn getter, SetBuilderFn setter, const std::string &key)
{
  configuration::Registry registry;
  ASSERT_EQ(std::invoke(getter, registry, key), nullptr);

  auto first            = std::make_unique<MockType>();
  const auto *first_ptr = first.get();
  std::invoke(setter, registry, key, std::move(first));
  ASSERT_EQ(std::invoke(getter, registry, key), first_ptr);
  ASSERT_EQ(std::invoke(getter, registry, "other"), nullptr);

  auto second            = std::make_unique<MockType>();
  const auto *second_ptr = second.get();
  std::invoke(setter, registry, key, std::move(second));
  ASSERT_EQ(std::invoke(getter, registry, key), second_ptr);
}

TEST(Registry, AlwaysOnSamplerBuilder)
{
  TestTypedSlot<TestAlwaysOnSamplerBuilder>(&configuration::Registry::GetAlwaysOnSamplerBuilder,
                                            &configuration::Registry::SetAlwaysOnSamplerBuilder);
}

TEST(Registry, AlwaysOffSamplerBuilder)
{
  TestTypedSlot<TestAlwaysOffSamplerBuilder>(&configuration::Registry::GetAlwaysOffSamplerBuilder,
                                             &configuration::Registry::SetAlwaysOffSamplerBuilder);
}

TEST(Registry, TraceIdRatioBasedSamplerBuilder)
{
  TestTypedSlot<TestTraceIdRatioBasedSamplerBuilder>(
      &configuration::Registry::GetTraceIdRatioBasedSamplerBuilder,
      &configuration::Registry::SetTraceIdRatioBasedSamplerBuilder);
}

TEST(Registry, ProbabilitySamplerBuilder)
{
  TestTypedSlot<TestProbabilitySamplerBuilder>(
      &configuration::Registry::GetProbabilitySamplerBuilder,
      &configuration::Registry::SetProbabilitySamplerBuilder);
}

TEST(Registry, ParentBasedSamplerBuilder)
{
  TestTypedSlot<TestParentBasedSamplerBuilder>(
      &configuration::Registry::GetParentBasedSamplerBuilder,
      &configuration::Registry::SetParentBasedSamplerBuilder);
}

TEST(Registry, JaegerRemoteSamplerBuilder)
{
  TestTypedSlot<TestJaegerRemoteSamplerBuilder>(
      &configuration::Registry::GetJaegerRemoteSamplerBuilder,
      &configuration::Registry::SetJaegerRemoteSamplerBuilder);
}

TEST(Registry, ComposableAlwaysOnSamplerBuilder)
{
  TestTypedSlot<TestComposableAlwaysOnSamplerBuilder>(
      &configuration::Registry::GetComposableAlwaysOnSamplerBuilder,
      &configuration::Registry::SetComposableAlwaysOnSamplerBuilder);
}

TEST(Registry, ComposableAlwaysOffSamplerBuilder)
{
  TestTypedSlot<TestComposableAlwaysOffSamplerBuilder>(
      &configuration::Registry::GetComposableAlwaysOffSamplerBuilder,
      &configuration::Registry::SetComposableAlwaysOffSamplerBuilder);
}

TEST(Registry, ComposableProbabilitySamplerBuilder)
{
  TestTypedSlot<TestComposableProbabilitySamplerBuilder>(
      &configuration::Registry::GetComposableProbabilitySamplerBuilder,
      &configuration::Registry::SetComposableProbabilitySamplerBuilder);
}

TEST(Registry, ComposableParentThresholdSamplerBuilder)
{
  TestTypedSlot<TestComposableParentThresholdSamplerBuilder>(
      &configuration::Registry::GetComposableParentThresholdSamplerBuilder,
      &configuration::Registry::SetComposableParentThresholdSamplerBuilder);
}

TEST(Registry, ComposableRuleBasedSamplerBuilder)
{
  TestTypedSlot<TestComposableRuleBasedSamplerBuilder>(
      &configuration::Registry::GetComposableRuleBasedSamplerBuilder,
      &configuration::Registry::SetComposableRuleBasedSamplerBuilder);
}

TEST(Registry, BatchSpanProcessorBuilder)
{
  TestTypedSlot<TestBatchSpanProcessorBuilder>(
      &configuration::Registry::GetBatchSpanProcessorBuilder,
      &configuration::Registry::SetBatchSpanProcessorBuilder);
}

TEST(Registry, SimpleSpanProcessorBuilder)
{
  TestTypedSlot<TestSimpleSpanProcessorBuilder>(
      &configuration::Registry::GetSimpleSpanProcessorBuilder,
      &configuration::Registry::SetSimpleSpanProcessorBuilder);
}

TEST(Registry, BatchLogRecordProcessorBuilder)
{
  TestTypedSlot<TestBatchLogRecordProcessorBuilder>(
      &configuration::Registry::GetBatchLogRecordProcessorBuilder,
      &configuration::Registry::SetBatchLogRecordProcessorBuilder);
}

TEST(Registry, SimpleLogRecordProcessorBuilder)
{
  TestTypedSlot<TestSimpleLogRecordProcessorBuilder>(
      &configuration::Registry::GetSimpleLogRecordProcessorBuilder,
      &configuration::Registry::SetSimpleLogRecordProcessorBuilder);
}

TEST(Registry, PullMetricReaderBuilder)
{
  TestTypedSlot<TestPullMetricReaderBuilder>(&configuration::Registry::GetPullMetricReaderBuilder,
                                             &configuration::Registry::SetPullMetricReaderBuilder);
}

TEST(Registry, TracerConfiguratorBuilder)
{
  TestTypedSlot<TestTracerConfiguratorBuilder>(
      &configuration::Registry::GetTracerConfiguratorBuilder,
      &configuration::Registry::SetTracerConfiguratorBuilder);
}

TEST(Registry, MeterConfiguratorBuilder)
{
  TestTypedSlot<TestMeterConfiguratorBuilder>(
      &configuration::Registry::GetMeterConfiguratorBuilder,
      &configuration::Registry::SetMeterConfiguratorBuilder);
}

TEST(Registry, LoggerConfiguratorBuilder)
{
  TestTypedSlot<TestLoggerConfiguratorBuilder>(
      &configuration::Registry::GetLoggerConfiguratorBuilder,
      &configuration::Registry::SetLoggerConfiguratorBuilder);
}

TEST(Registry, OpenCensusMetricProducerBuilder)
{
  TestTypedSlot<TestOpenCensusMetricProducerBuilder>(
      &configuration::Registry::GetOpenCensusMetricProducerBuilder,
      &configuration::Registry::SetOpenCensusMetricProducerBuilder);
}

TEST(Registry, ExtensionMetricProducerBuilder)
{
  TestNamedSlot<TestExtensionMetricProducerBuilder>(
      &configuration::Registry::GetExtensionMetricProducerBuilder,
      &configuration::Registry::SetExtensionMetricProducerBuilder, "my_producer");
}

TEST(Registry, ContainerResourceDetectorBuilder)
{
  TestTypedSlot<TestContainerResourceDetectorBuilder>(
      &configuration::Registry::GetContainerResourceDetectorBuilder,
      &configuration::Registry::SetContainerResourceDetectorBuilder);
}

TEST(Registry, HostResourceDetectorBuilder)
{
  TestTypedSlot<TestHostResourceDetectorBuilder>(
      &configuration::Registry::GetHostResourceDetectorBuilder,
      &configuration::Registry::SetHostResourceDetectorBuilder);
}

TEST(Registry, ProcessResourceDetectorBuilder)
{
  TestTypedSlot<TestProcessResourceDetectorBuilder>(
      &configuration::Registry::GetProcessResourceDetectorBuilder,
      &configuration::Registry::SetProcessResourceDetectorBuilder);
}

TEST(Registry, ServiceResourceDetectorBuilder)
{
  TestTypedSlot<TestServiceResourceDetectorBuilder>(
      &configuration::Registry::GetServiceResourceDetectorBuilder,
      &configuration::Registry::SetServiceResourceDetectorBuilder);
}

TEST(Registry, ExtensionResourceDetectorBuilder)
{
  TestNamedSlot<TestExtensionResourceDetectorBuilder>(
      &configuration::Registry::GetExtensionResourceDetectorBuilder,
      &configuration::Registry::SetExtensionResourceDetectorBuilder, "my_detector");
}
