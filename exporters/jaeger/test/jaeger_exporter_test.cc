// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <opentelemetry/exporters/jaeger/jaeger_exporter.h>
#include <memory>
#include <vector>
#include "opentelemetry/sdk/trace/batch_span_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"

#ifdef BAZEL_BUILD
#  include "exporters/jaeger/src/thrift_sender.h"
#else
#  include "thrift_sender.h"
#endif

#include <gtest/gtest.h>
#include "gmock/gmock.h"

namespace trace      = opentelemetry::trace;
namespace nostd      = opentelemetry::nostd;
namespace sdktrace   = opentelemetry::sdk::trace;
namespace common     = opentelemetry::common;
namespace sdk_common = opentelemetry::sdk::common;

using namespace testing;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

namespace trace_api = opentelemetry::trace;
namespace resource  = opentelemetry::sdk::resource;

template <class T, size_t N>
static nostd::span<T, N> MakeSpan(T (&array)[N])
{
  return nostd::span<T, N>(array);
}

class JaegerExporterTestPeer : public ::testing::Test
{
public:
  std::unique_ptr<sdk::trace::SpanExporter> GetExporter(std::unique_ptr<ThriftSender> sender)
  {
    return std::unique_ptr<sdk::trace::SpanExporter>(new JaegerExporter(std::move(sender)));
  }

  // Get the options associated with the given exporter.
  const JaegerExporterOptions &GetOptions(std::unique_ptr<JaegerExporter> &exporter)
  {
    return exporter->options_;
  }
};

class MockThriftSender : public ThriftSender
{
public:
  MOCK_METHOD(int, Append, (std::unique_ptr<JaegerRecordable> &&), (noexcept, override));
};

class MockTransport : public Transport
{
public:
  MOCK_METHOD(int, EmitBatch, (const thrift::Batch &), (override));
  MOCK_METHOD(uint32_t, MaxPacketSize, (), (const, override));
};

// Create spans, let processor call Export()
TEST_F(JaegerExporterTestPeer, ExportIntegrationTest)
{
  auto mock_transport     = new MockTransport;
  auto mock_thrift_sender = new ThriftSender(std::unique_ptr<MockTransport>{mock_transport});
  auto exporter           = GetExporter(std::unique_ptr<ThriftSender>{mock_thrift_sender});

  resource::ResourceAttributes resource_attributes = {{"service.name", "unit_test_service"},
                                                      {"tenant.id", "test_user"}};
  resource_attributes["bool_value"]                = true;
  resource_attributes["int32_value"]               = static_cast<int32_t>(1);
  resource_attributes["uint32_value"]              = static_cast<uint32_t>(2);
  resource_attributes["int64_value"]               = static_cast<int64_t>(0x1100000000LL);
  resource_attributes["double_value"]              = static_cast<double>(3.1);
  auto resource = resource::Resource::Create(resource_attributes);

  auto processor_opts                  = sdk::trace::BatchSpanProcessorOptions();
  processor_opts.max_export_batch_size = 5;
  processor_opts.max_queue_size        = 5;
  processor_opts.schedule_delay_millis = std::chrono::milliseconds(256);
  auto processor                       = std::unique_ptr<sdk::trace::SpanProcessor>(
      new sdk::trace::BatchSpanProcessor(std::move(exporter), processor_opts));
  auto provider = nostd::shared_ptr<trace::TracerProvider>(
      new sdk::trace::TracerProvider(std::move(processor), resource));

  EXPECT_CALL(*mock_transport, EmitBatch(_)).Times(Exactly(1)).WillOnce(Return(1));

  auto tracer      = provider->GetTracer("test");
  auto parent_span = tracer->StartSpan("Test parent span");

  trace_api::StartSpanOptions child_span_opts = {};
  child_span_opts.parent                      = parent_span->GetContext();
  auto child_span = tracer->StartSpan("Test child span", child_span_opts);

  child_span->End();
  parent_span->End();

  auto parent_ctx = parent_span->GetContext();
  auto child_ctx  = child_span->GetContext();
  EXPECT_EQ(parent_ctx.trace_id(), child_ctx.trace_id());
  EXPECT_EQ(parent_ctx.trace_state(), child_ctx.trace_state());
  ASSERT_TRUE(parent_ctx.IsValid());
  ASSERT_TRUE(child_ctx.IsValid());
}

TEST_F(JaegerExporterTestPeer, ShutdownTest)
{
  auto mock_thrift_sender = new MockThriftSender;
  auto exporter           = GetExporter(std::unique_ptr<ThriftSender>{mock_thrift_sender});

  auto recordable_1 = exporter->MakeRecordable();
  recordable_1->SetName("Test span 1");
  auto recordable_2 = exporter->MakeRecordable();
  recordable_2->SetName("Test span 2");

  // exporter shuold not be shutdown by default
  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch_1(&recordable_1, 1);
  EXPECT_CALL(*mock_thrift_sender, Append(_)).Times(Exactly(1)).WillOnce(Return(1));
  auto result = exporter->Export(batch_1);
  EXPECT_EQ(sdk_common::ExportResult::kSuccess, result);

  exporter->Shutdown();

  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch_2(&recordable_2, 1);
  result = exporter->Export(batch_2);
  EXPECT_EQ(sdk_common::ExportResult::kFailure, result);
}

// Call Export() directly
TEST_F(JaegerExporterTestPeer, ExportTest)
{
  auto mock_thrift_sender = new MockThriftSender;
  auto exporter           = GetExporter(std::unique_ptr<ThriftSender>{mock_thrift_sender});

  auto recordable_1 = exporter->MakeRecordable();
  recordable_1->SetName("Test span 1");
  auto recordable_2 = exporter->MakeRecordable();
  recordable_2->SetName("Test span 2");

  // Test successful send
  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch_1(&recordable_1, 1);
  EXPECT_CALL(*mock_thrift_sender, Append(_)).Times(Exactly(1)).WillOnce(Return(1));
  auto result = exporter->Export(batch_1);
  EXPECT_EQ(sdk_common::ExportResult::kSuccess, result);

  // Test failed send
  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch_2(&recordable_2, 1);
  EXPECT_CALL(*mock_thrift_sender, Append(_)).Times(Exactly(1)).WillOnce(Return(0));
  result = exporter->Export(batch_2);
  EXPECT_EQ(sdk::common::ExportResult::kFailure, result);
}

// Test exporter configuration options
TEST_F(JaegerExporterTestPeer, ConfigTest)
{
  JaegerExporterOptions opts;
  opts.endpoint    = "localhost";
  opts.server_port = 6851;
  std::unique_ptr<JaegerExporter> exporter(new JaegerExporter(opts));
  EXPECT_EQ(GetOptions(exporter).endpoint, "localhost");
  EXPECT_EQ(GetOptions(exporter).server_port, 6851);
}

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
