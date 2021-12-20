// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <opentelemetry/exporters/jaeger/jaeger_exporter.h>
#include <memory>
#include <vector>

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

TEST_F(JaegerExporterTestPeer, ShutdownTest)
{
  auto mock_thrift_sender = new MockThriftSender;
  auto exporter          = GetExporter(std::unique_ptr<ThriftSender>{mock_thrift_sender});

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
  auto exporter          = GetExporter(std::unique_ptr<ThriftSender>{mock_thrift_sender});

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
