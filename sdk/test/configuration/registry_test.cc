// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
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
#include "opentelemetry/sdk/configuration/console_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/console_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/container_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/extension_metric_producer_builder.h"
#include "opentelemetry/sdk/configuration/extension_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/extension_sampler_builder.h"
#include "opentelemetry/sdk/configuration/extension_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/extension_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/host_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/jaeger_remote_sampler_builder.h"
#include "opentelemetry/sdk/configuration/logger_configurator_builder.h"
#include "opentelemetry/sdk/configuration/meter_configurator_builder.h"
#include "opentelemetry/sdk/configuration/open_census_metric_producer_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_file_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_grpc_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_log_record_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_push_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/otlp_http_span_exporter_builder.h"
#include "opentelemetry/sdk/configuration/parent_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/periodic_metric_reader_builder.h"
#include "opentelemetry/sdk/configuration/probability_sampler_builder.h"
#include "opentelemetry/sdk/configuration/process_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/prometheus_pull_metric_exporter_builder.h"
#include "opentelemetry/sdk/configuration/pull_metric_reader_builder.h"
#include "opentelemetry/sdk/configuration/registry.h"
#include "opentelemetry/sdk/configuration/service_resource_detector_builder.h"
#include "opentelemetry/sdk/configuration/simple_log_record_processor_builder.h"
#include "opentelemetry/sdk/configuration/simple_span_processor_builder.h"
#include "opentelemetry/sdk/configuration/text_map_propagator_builder.h"
#include "opentelemetry/sdk/configuration/trace_id_ratio_based_sampler_builder.h"
#include "opentelemetry/sdk/configuration/tracer_configurator_builder.h"

#include "opentelemetry/context/propagation/text_map_propagator.h"      // IWYU pragma: keep
#include "opentelemetry/sdk/instrumentationscope/scope_configurator.h"  // IWYU pragma: keep
#include "opentelemetry/sdk/logs/exporter.h"                            // IWYU pragma: keep
#include "opentelemetry/sdk/logs/logger_config.h"                       // IWYU pragma: keep
#include "opentelemetry/sdk/logs/processor.h"                           // IWYU pragma: keep
#include "opentelemetry/sdk/metrics/export/metric_producer.h"           // IWYU pragma: keep
#include "opentelemetry/sdk/metrics/meter_config.h"                     // IWYU pragma: keep
#include "opentelemetry/sdk/metrics/metric_reader.h"                    // IWYU pragma: keep
#include "opentelemetry/sdk/metrics/push_metric_exporter.h"             // IWYU pragma: keep
#include "opentelemetry/sdk/resource/resource_detector.h"               // IWYU pragma: keep
#include "opentelemetry/sdk/trace/exporter.h"                           // IWYU pragma: keep
#include "opentelemetry/sdk/trace/processor.h"                          // IWYU pragma: keep
#include "opentelemetry/sdk/trace/sampler.h"                            // IWYU pragma: keep
#include "opentelemetry/sdk/trace/samplers/composable_sampler.h"        // IWYU pragma: keep
#include "opentelemetry/sdk/trace/tracer_config.h"                      // IWYU pragma: keep

namespace configuration = opentelemetry::sdk::configuration;

namespace
{

// ----------------------------------------------------------------------------------
// Test SDK Component Builders

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
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> &&root,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> &&remote_parent_sampled,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> &&remote_parent_not_sampled,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> &&local_parent_sampled,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> &&local_parent_not_sampled) const override
  {
    auto unused_root                      = std::move(root);
    auto unused_remote_parent_sampled     = std::move(remote_parent_sampled);
    auto unused_remote_parent_not_sampled = std::move(remote_parent_not_sampled);
    auto unused_local_parent_sampled      = std::move(local_parent_sampled);
    auto unused_local_parent_not_sampled  = std::move(local_parent_not_sampled);
    return nullptr;
  }
};

class TestJaegerRemoteSamplerBuilder : public configuration::JaegerRemoteSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const configuration::JaegerRemoteSamplerConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::trace::Sampler> &&initial_sampler) const override
  {
    auto unused = std::move(initial_sampler);
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
      std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> &&root) const override
  {
    auto unused = std::move(root);
    return nullptr;
  }
};

class TestComposableRuleBasedSamplerBuilder
    : public configuration::ComposableRuleBasedSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler> Build(
      const configuration::ComposableRuleBasedSamplerConfiguration * /* model */,
      std::vector<std::unique_ptr<opentelemetry::sdk::trace::ComposableSampler>> &&rule_samplers)
      const override
  {
    auto unused = std::move(rule_samplers);
    return nullptr;
  }
};

class TestBatchSpanProcessorBuilder : public configuration::BatchSpanProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> Build(
      const configuration::BatchSpanProcessorConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> &&exporter) const override
  {
    auto unused = std::move(exporter);
    return nullptr;
  }
};

class TestSimpleSpanProcessorBuilder : public configuration::SimpleSpanProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> Build(
      const configuration::SimpleSpanProcessorConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> &&exporter) const override
  {
    auto unused = std::move(exporter);
    return nullptr;
  }
};

class TestBatchLogRecordProcessorBuilder : public configuration::BatchLogRecordProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> Build(
      const configuration::BatchLogRecordProcessorConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> &&exporter) const override
  {
    auto unused = std::move(exporter);
    return nullptr;
  }
};

class TestSimpleLogRecordProcessorBuilder : public configuration::SimpleLogRecordProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> Build(
      const configuration::SimpleLogRecordProcessorConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> &&exporter) const override
  {
    auto unused = std::move(exporter);
    return nullptr;
  }
};

class TestPullMetricReaderBuilder : public configuration::PullMetricReaderBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> Build(
      const configuration::PullMetricReaderConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> &&reader) const override
  {
    auto unused = std::move(reader);
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

class TestConsoleSpanExporterBuilder : public configuration::ConsoleSpanExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const configuration::ConsoleSpanExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestConsolePushMetricExporterBuilder : public configuration::ConsolePushMetricExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Build(
      const configuration::ConsolePushMetricExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestConsoleLogRecordExporterBuilder : public configuration::ConsoleLogRecordExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const configuration::ConsoleLogRecordExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestOtlpHttpSpanExporterBuilder : public configuration::OtlpHttpSpanExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const configuration::OtlpHttpSpanExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestOtlpGrpcSpanExporterBuilder : public configuration::OtlpGrpcSpanExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const configuration::OtlpGrpcSpanExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestOtlpFileSpanExporterBuilder : public configuration::OtlpFileSpanExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const configuration::OtlpFileSpanExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestOtlpHttpPushMetricExporterBuilder
    : public configuration::OtlpHttpPushMetricExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Build(
      const configuration::OtlpHttpPushMetricExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestOtlpGrpcPushMetricExporterBuilder
    : public configuration::OtlpGrpcPushMetricExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Build(
      const configuration::OtlpGrpcPushMetricExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestOtlpFilePushMetricExporterBuilder
    : public configuration::OtlpFilePushMetricExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Build(
      const configuration::OtlpFilePushMetricExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestOtlpHttpLogRecordExporterBuilder : public configuration::OtlpHttpLogRecordExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const configuration::OtlpHttpLogRecordExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestOtlpGrpcLogRecordExporterBuilder : public configuration::OtlpGrpcLogRecordExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const configuration::OtlpGrpcLogRecordExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestOtlpFileLogRecordExporterBuilder : public configuration::OtlpFileLogRecordExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const configuration::OtlpFileLogRecordExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestPeriodicMetricReaderBuilder : public configuration::PeriodicMetricReaderBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> Build(
      const configuration::PeriodicMetricReaderConfiguration * /* model */,
      std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> &&exporter) const override
  {
    auto unused = std::move(exporter);
    return nullptr;
  }
};

class TestPrometheusPullMetricExporterBuilder
    : public configuration::PrometheusPullMetricExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> Build(
      const configuration::PrometheusPullMetricExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestTextMapPropagatorBuilder : public configuration::TextMapPropagatorBuilder
{
public:
  std::unique_ptr<opentelemetry::context::propagation::TextMapPropagator> Build() const override
  {
    return nullptr;
  }
};

class TestExtensionSamplerBuilder : public configuration::ExtensionSamplerBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::Sampler> Build(
      const configuration::ExtensionSamplerConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestExtensionSpanExporterBuilder : public configuration::ExtensionSpanExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Build(
      const configuration::ExtensionSpanExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestExtensionSpanProcessorBuilder : public configuration::ExtensionSpanProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::trace::SpanProcessor> Build(
      const configuration::ExtensionSpanProcessorConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestExtensionPushMetricExporterBuilder
    : public configuration::ExtensionPushMetricExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::PushMetricExporter> Build(
      const configuration::ExtensionPushMetricExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestExtensionPullMetricExporterBuilder
    : public configuration::ExtensionPullMetricExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::metrics::MetricReader> Build(
      const configuration::ExtensionPullMetricExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestExtensionLogRecordExporterBuilder
    : public configuration::ExtensionLogRecordExporterBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> Build(
      const configuration::ExtensionLogRecordExporterConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

class TestExtensionLogRecordProcessorBuilder
    : public configuration::ExtensionLogRecordProcessorBuilder
{
public:
  std::unique_ptr<opentelemetry::sdk::logs::LogRecordProcessor> Build(
      const configuration::ExtensionLogRecordProcessorConfiguration * /* model */) const override
  {
    return nullptr;
  }
};

}  // namespace

// ----------------------------------------------------------------------------------
// Test Helpers

namespace
{

template <typename MockType, typename GetBuilderFn, typename SetBuilderFn>
void TestTypedSlot(GetBuilderFn getter, SetBuilderFn setter)
{
  configuration::Registry registry;
  ASSERT_EQ((registry.*getter)(), nullptr);

  auto first            = std::make_unique<MockType>();
  const auto *first_ptr = first.get();
  (registry.*setter)(std::move(first));
  ASSERT_EQ((registry.*getter)(), first_ptr);

  auto second            = std::make_unique<MockType>();
  const auto *second_ptr = second.get();
  (registry.*setter)(std::move(second));
  ASSERT_EQ((registry.*getter)(), second_ptr);
}

template <typename MockType, typename GetBuilderFn, typename SetBuilderFn>
void TestNamedSlot(GetBuilderFn getter, SetBuilderFn setter, const std::string &key)
{
  configuration::Registry registry;
  ASSERT_EQ((registry.*getter)(key), nullptr);

  auto first            = std::make_unique<MockType>();
  const auto *first_ptr = first.get();
  (registry.*setter)(key, std::move(first));
  ASSERT_EQ((registry.*getter)(key), first_ptr);
  ASSERT_EQ((registry.*getter)("other"), nullptr);

  auto second            = std::make_unique<MockType>();
  const auto *second_ptr = second.get();
  (registry.*setter)(key, std::move(second));
  ASSERT_EQ((registry.*getter)(key), second_ptr);
}

}  // namespace

// ----------------------------------------------------------------------------------
// Test Cases

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

TEST(Registry, ConsoleSpanBuilder)
{
  TestTypedSlot<TestConsoleSpanExporterBuilder>(&configuration::Registry::GetConsoleSpanBuilder,
                                                &configuration::Registry::SetConsoleSpanBuilder);
}

TEST(Registry, ConsolePushMetricExporterBuilder)
{
  TestTypedSlot<TestConsolePushMetricExporterBuilder>(
      &configuration::Registry::GetConsolePushMetricExporterBuilder,
      &configuration::Registry::SetConsolePushMetricExporterBuilder);
}

TEST(Registry, ConsoleLogRecordBuilder)
{
  TestTypedSlot<TestConsoleLogRecordExporterBuilder>(
      &configuration::Registry::GetConsoleLogRecordBuilder,
      &configuration::Registry::SetConsoleLogRecordBuilder);
}

TEST(Registry, OtlpHttpSpanBuilder)
{
  TestTypedSlot<TestOtlpHttpSpanExporterBuilder>(&configuration::Registry::GetOtlpHttpSpanBuilder,
                                                 &configuration::Registry::SetOtlpHttpSpanBuilder);
}

TEST(Registry, OtlpGrpcSpanBuilder)
{
  TestTypedSlot<TestOtlpGrpcSpanExporterBuilder>(&configuration::Registry::GetOtlpGrpcSpanBuilder,
                                                 &configuration::Registry::SetOtlpGrpcSpanBuilder);
}

TEST(Registry, OtlpFileSpanBuilder)
{
  TestTypedSlot<TestOtlpFileSpanExporterBuilder>(&configuration::Registry::GetOtlpFileSpanBuilder,
                                                 &configuration::Registry::SetOtlpFileSpanBuilder);
}

TEST(Registry, OtlpHttpPushMetricExporterBuilder)
{
  TestTypedSlot<TestOtlpHttpPushMetricExporterBuilder>(
      &configuration::Registry::GetOtlpHttpPushMetricExporterBuilder,
      &configuration::Registry::SetOtlpHttpPushMetricExporterBuilder);
}

TEST(Registry, OtlpGrpcPushMetricExporterBuilder)
{
  TestTypedSlot<TestOtlpGrpcPushMetricExporterBuilder>(
      &configuration::Registry::GetOtlpGrpcPushMetricExporterBuilder,
      &configuration::Registry::SetOtlpGrpcPushMetricExporterBuilder);
}

TEST(Registry, OtlpFilePushMetricExporterBuilder)
{
  TestTypedSlot<TestOtlpFilePushMetricExporterBuilder>(
      &configuration::Registry::GetOtlpFilePushMetricExporterBuilder,
      &configuration::Registry::SetOtlpFilePushMetricExporterBuilder);
}

TEST(Registry, OtlpHttpLogRecordBuilder)
{
  TestTypedSlot<TestOtlpHttpLogRecordExporterBuilder>(
      &configuration::Registry::GetOtlpHttpLogRecordBuilder,
      &configuration::Registry::SetOtlpHttpLogRecordBuilder);
}

TEST(Registry, OtlpGrpcLogRecordBuilder)
{
  TestTypedSlot<TestOtlpGrpcLogRecordExporterBuilder>(
      &configuration::Registry::GetOtlpGrpcLogRecordBuilder,
      &configuration::Registry::SetOtlpGrpcLogRecordBuilder);
}

TEST(Registry, OtlpFileLogRecordBuilder)
{
  TestTypedSlot<TestOtlpFileLogRecordExporterBuilder>(
      &configuration::Registry::GetOtlpFileLogRecordBuilder,
      &configuration::Registry::SetOtlpFileLogRecordBuilder);
}

TEST(Registry, PeriodicMetricReaderBuilder)
{
  // Registry pre-populates this slot; test replace lifecycle only.
  configuration::Registry registry;
  ASSERT_NE(registry.GetPeriodicMetricReaderBuilder(), nullptr);

  auto first            = std::make_unique<TestPeriodicMetricReaderBuilder>();
  const auto *first_ptr = first.get();
  registry.SetPeriodicMetricReaderBuilder(std::move(first));
  ASSERT_EQ(registry.GetPeriodicMetricReaderBuilder(), first_ptr);

  auto second            = std::make_unique<TestPeriodicMetricReaderBuilder>();
  const auto *second_ptr = second.get();
  registry.SetPeriodicMetricReaderBuilder(std::move(second));
  ASSERT_EQ(registry.GetPeriodicMetricReaderBuilder(), second_ptr);
}

TEST(Registry, PrometheusPullMetricExporterBuilder)
{
  TestTypedSlot<TestPrometheusPullMetricExporterBuilder>(
      &configuration::Registry::GetPrometheusPullMetricExporterBuilder,
      &configuration::Registry::SetPrometheusPullMetricExporterBuilder);
}

TEST(Registry, TextMapPropagatorBuilder)
{
  // Registry pre-populates known propagator names; use an unknown key.
  configuration::Registry registry;
  ASSERT_EQ(registry.GetTextMapPropagatorBuilder("custom_propagator"), nullptr);

  auto first            = std::make_unique<TestTextMapPropagatorBuilder>();
  const auto *first_ptr = first.get();
  registry.SetTextMapPropagatorBuilder("custom_propagator", std::move(first));
  ASSERT_EQ(registry.GetTextMapPropagatorBuilder("custom_propagator"), first_ptr);
  ASSERT_EQ(registry.GetTextMapPropagatorBuilder("other"), nullptr);

  auto second            = std::make_unique<TestTextMapPropagatorBuilder>();
  const auto *second_ptr = second.get();
  registry.SetTextMapPropagatorBuilder("custom_propagator", std::move(second));
  ASSERT_EQ(registry.GetTextMapPropagatorBuilder("custom_propagator"), second_ptr);
}

TEST(Registry, ExtensionSamplerBuilder)
{
  TestNamedSlot<TestExtensionSamplerBuilder>(&configuration::Registry::GetExtensionSamplerBuilder,
                                             &configuration::Registry::SetExtensionSamplerBuilder,
                                             "my_sampler");
}

TEST(Registry, ExtensionSpanExporterBuilder)
{
  TestNamedSlot<TestExtensionSpanExporterBuilder>(
      &configuration::Registry::GetExtensionSpanExporterBuilder,
      &configuration::Registry::SetExtensionSpanExporterBuilder, "my_exporter");
}

TEST(Registry, ExtensionSpanProcessorBuilder)
{
  TestNamedSlot<TestExtensionSpanProcessorBuilder>(
      &configuration::Registry::GetExtensionSpanProcessorBuilder,
      &configuration::Registry::SetExtensionSpanProcessorBuilder, "my_processor");
}

TEST(Registry, ExtensionPushMetricExporterBuilder)
{
  TestNamedSlot<TestExtensionPushMetricExporterBuilder>(
      &configuration::Registry::GetExtensionPushMetricExporterBuilder,
      &configuration::Registry::SetExtensionPushMetricExporterBuilder, "my_exporter");
}

TEST(Registry, ExtensionPullMetricExporterBuilder)
{
  TestNamedSlot<TestExtensionPullMetricExporterBuilder>(
      &configuration::Registry::GetExtensionPullMetricExporterBuilder,
      &configuration::Registry::SetExtensionPullMetricExporterBuilder, "my_exporter");
}

TEST(Registry, ExtensionLogRecordExporterBuilder)
{
  TestNamedSlot<TestExtensionLogRecordExporterBuilder>(
      &configuration::Registry::GetExtensionLogRecordExporterBuilder,
      &configuration::Registry::SetExtensionLogRecordExporterBuilder, "my_exporter");
}

TEST(Registry, ExtensionLogRecordProcessorBuilder)
{
  TestNamedSlot<TestExtensionLogRecordProcessorBuilder>(
      &configuration::Registry::GetExtensionLogRecordProcessorBuilder,
      &configuration::Registry::SetExtensionLogRecordProcessorBuilder, "my_processor");
}
