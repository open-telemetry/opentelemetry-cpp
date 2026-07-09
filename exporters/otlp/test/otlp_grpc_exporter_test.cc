// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <grpcpp/grpcpp.h>
#include <gtest/gtest.h>
#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "gmock/gmock.h"

#include "opentelemetry/exporters/otlp/otlp_grpc_exporter.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"
#include "opentelemetry/sdk/common/exporter_utils.h"
#include "opentelemetry/sdk/common/global_log_handler.h"
#include "opentelemetry/sdk/trace/exporter.h"
#include "opentelemetry/sdk/trace/processor.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/simple_processor.h"
#include "opentelemetry/sdk/trace/tracer_provider.h"
#include "opentelemetry/test_common/sdk/common/scoped_test_log_handler.h"
#include "opentelemetry/trace/span.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/trace/tracer_provider.h"
#include "opentelemetry/version.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
#include "opentelemetry/proto/collector/trace/v1/trace_service_mock.grpc.pb.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.grpc.pb.h"
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h" // IWYU pragma: keep
// clang-format on

// IWYU pragma: no_include <grpc/support/port_platform.h>
// IWYU pragma: no_include <grpcpp/support/status.h>

#if defined(_MSC_VER)
#  include "opentelemetry/sdk/common/env_variables.h"
using opentelemetry::sdk::common::setenv;
using opentelemetry::sdk::common::unsetenv;
#endif

using namespace testing;

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

namespace
{
class OtlpMockTraceServiceStub : public proto::collector::trace::v1::MockTraceServiceStub
{
public:
// Some old toolchains can only use gRPC 1.33 and it's experimental.
#if defined(GRPC_CPP_VERSION_MAJOR) && \
    (GRPC_CPP_VERSION_MAJOR * 1000 + GRPC_CPP_VERSION_MINOR) >= 1039
  using async_interface_base =
      proto::collector::trace::v1::TraceService::StubInterface::async_interface;
#else
  using async_interface_base =
      proto::collector::trace::v1::TraceService::StubInterface::experimental_async_interface;
#endif

  OtlpMockTraceServiceStub() : async_interface_(this) {}

  class async_interface : public async_interface_base
  {
  public:
    async_interface(OtlpMockTraceServiceStub *owner) : stub_(owner) {}

    void Export(
        ::grpc::ClientContext *context,
        const ::opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest *request,
        ::opentelemetry::proto::collector::trace::v1::ExportTraceServiceResponse *response,
        std::function<void(::grpc::Status)> callback) override
    {
      stub_->last_async_status_ = stub_->Export(context, *request, response);
      callback(stub_->last_async_status_);
    }

// Some old toolchains can only use gRPC 1.33 and it's experimental.
#if defined(GRPC_CPP_VERSION_MAJOR) &&                                      \
        (GRPC_CPP_VERSION_MAJOR * 1000 + GRPC_CPP_VERSION_MINOR) >= 1039 || \
    defined(GRPC_CALLBACK_API_NONEXPERIMENTAL)
    void Export(
        ::grpc::ClientContext * /*context*/,
        const ::opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest * /*request*/,
        ::opentelemetry::proto::collector::trace::v1::ExportTraceServiceResponse * /*response*/,
        ::grpc::ClientUnaryReactor * /*reactor*/) override
    {}
#else
    void Export(
        ::grpc::ClientContext * /*context*/,
        const ::opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest * /*request*/,
        ::opentelemetry::proto::collector::trace::v1::ExportTraceServiceResponse * /*response*/,
        ::grpc::experimental::ClientUnaryReactor * /*reactor*/)
    {}
#endif

  private:
    OtlpMockTraceServiceStub *stub_;
  };

  async_interface_base *async() override { return &async_interface_; }
  async_interface_base *experimental_async() { return &async_interface_; }

  ::grpc::Status GetLastAsyncStatus() const noexcept { return last_async_status_; }

private:
  ::grpc::Status last_async_status_;
  async_interface async_interface_;
};

using opentelemetry::test_common::ScopedTestLogHandler;
}  // namespace

class OtlpGrpcExporterTestPeer : public ::testing::Test
{
public:
  std::unique_ptr<sdk::trace::SpanExporter> GetExporter(
      std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> &stub_interface)
  {
    return std::unique_ptr<sdk::trace::SpanExporter>(
        new OtlpGrpcExporter(std::move(stub_interface)));
  }

  // Get the options associated with the given exporter.
  const OtlpGrpcExporterOptions &GetOptions(std::unique_ptr<OtlpGrpcExporter> &exporter)
  {
    return exporter->options_;
  }
};

TEST_F(OtlpGrpcExporterTestPeer, ShutdownTest)
{
  auto mock_stub = new OtlpMockTraceServiceStub();
  std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub_interface(
      mock_stub);
  auto exporter = GetExporter(stub_interface);

  auto recordable_1 = exporter->MakeRecordable();
  recordable_1->SetName("Test span 1");
  auto recordable_2 = exporter->MakeRecordable();
  recordable_2->SetName("Test span 2");

  // exporter shuold not be shutdown by default
  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch_1(&recordable_1, 1);
  EXPECT_CALL(*mock_stub, Export(_, _, _)).Times(Exactly(1)).WillOnce(Return(grpc::Status::OK));
  auto result = exporter->Export(batch_1);
  exporter->ForceFlush();
  EXPECT_EQ(sdk::common::ExportResult::kSuccess, result);

  exporter->Shutdown();

  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch_2(&recordable_2, 1);
  result = exporter->Export(batch_2);
  EXPECT_EQ(sdk::common::ExportResult::kFailure, result);
}

// Call Export() directly
TEST_F(OtlpGrpcExporterTestPeer, ExportUnitTest)
{
  auto mock_stub = new OtlpMockTraceServiceStub();
  std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub_interface(
      mock_stub);
  auto exporter = GetExporter(stub_interface);

  auto recordable_1 = exporter->MakeRecordable();
  recordable_1->SetName("Test span 1");
  auto recordable_2 = exporter->MakeRecordable();
  recordable_2->SetName("Test span 2");

  // Test successful RPC
  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch_1(&recordable_1, 1);
  EXPECT_CALL(*mock_stub, Export(_, _, _)).Times(Exactly(1)).WillOnce(Return(grpc::Status::OK));
  auto result = exporter->Export(batch_1);
  EXPECT_EQ(sdk::common::ExportResult::kSuccess, result);

  // Test failed RPC
  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch_2(&recordable_2, 1);
  EXPECT_CALL(*mock_stub, Export(_, _, _))
      .Times(Exactly(1))
      .WillOnce(Return(grpc::Status::CANCELLED));
  result = exporter->Export(batch_2);
  exporter->ForceFlush();
#if defined(ENABLE_ASYNC_EXPORT)
  EXPECT_EQ(sdk::common::ExportResult::kSuccess, result);
  EXPECT_FALSE(mock_stub->GetLastAsyncStatus().ok());
#else
  EXPECT_EQ(sdk::common::ExportResult::kFailure, result);
#endif
}

// Exporter logs the rejection on partial_success.
TEST_F(OtlpGrpcExporterTestPeer, ExportPartialSuccess)
{
  ScopedTestLogHandler log{sdk::common::internal_log::LogLevel::Error};

  auto mock_stub = new OtlpMockTraceServiceStub();
  std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub_interface(
      mock_stub);
  auto exporter = GetExporter(stub_interface);

  auto recordable = exporter->MakeRecordable();
  recordable->SetName("Test span");

  nostd::span<std::unique_ptr<sdk::trace::Recordable>> batch(&recordable, 1);
  EXPECT_CALL(*mock_stub, Export(_, _, _))
      .Times(Exactly(1))
      .WillOnce(Invoke(
          [](::grpc::ClientContext *,
             const proto::collector::trace::v1::ExportTraceServiceRequest &,
             proto::collector::trace::v1::ExportTraceServiceResponse *response) -> ::grpc::Status {
            response->mutable_partial_success()->set_rejected_spans(21);
            response->mutable_partial_success()->set_error_message("too many spans!!");
            return ::grpc::Status::OK;
          }));

  auto result = exporter->Export(batch);
  exporter->ForceFlush();

  EXPECT_EQ(sdk::common::ExportResult::kSuccess, result);

  auto entries  = log.Drain();
  auto contains = [&](const std::string &needle) {
    return std::any_of(entries.begin(), entries.end(), [&](const ScopedTestLogHandler::Entry &e) {
      return e.msg.find(needle) != std::string::npos;
    });
  };
  EXPECT_TRUE(contains("partial success"));
  EXPECT_TRUE(contains("21 span(s) rejected"));
  EXPECT_TRUE(contains("too many spans!!"));
}

// Create spans, let processor call Export()
TEST_F(OtlpGrpcExporterTestPeer, ExportIntegrationTest)
{
  auto mock_stub = new OtlpMockTraceServiceStub();
  std::unique_ptr<proto::collector::trace::v1::TraceService::StubInterface> stub_interface(
      mock_stub);

  auto exporter = GetExporter(stub_interface);

  std::unique_ptr<sdk::trace::SpanProcessor> processor =
      std::make_unique<sdk::trace::SimpleSpanProcessor>(std::move(exporter));
  auto provider = nostd::shared_ptr<trace::TracerProvider>(
      new sdk::trace::TracerProvider(std::move(processor)));
  auto tracer = provider->GetTracer("test");

  EXPECT_CALL(*mock_stub, Export(_, _, _))
      .Times(AtLeast(1))
      .WillRepeatedly(Return(grpc::Status::OK));

  auto parent_span = tracer->StartSpan("Test parent span");
  auto child_span  = tracer->StartSpan("Test child span");
  child_span->End();
  parent_span->End();
}

// Test exporter configuration options
TEST_F(OtlpGrpcExporterTestPeer, ConfigTest)
{
  OtlpGrpcExporterOptions opts;
  opts.endpoint                              = "localhost:45454";
  std::unique_ptr<OtlpGrpcExporter> exporter = std::make_unique<OtlpGrpcExporter>(opts);
  EXPECT_EQ(GetOptions(exporter).endpoint, "localhost:45454");
}

// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpGrpcExporterTestPeer, ConfigSslCredentialsTest)
{
  std::string cacert_str = "--begin and end fake cert--";
  OtlpGrpcExporterOptions opts;
  opts.use_ssl_credentials                   = true;
  opts.ssl_credentials_cacert_as_string      = cacert_str;
  std::unique_ptr<OtlpGrpcExporter> exporter = std::make_unique<OtlpGrpcExporter>(opts);
  EXPECT_EQ(GetOptions(exporter).ssl_credentials_cacert_as_string, cacert_str);
  EXPECT_EQ(GetOptions(exporter).use_ssl_credentials, true);
}

#ifndef NO_GETENV
// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpGrpcExporterTestPeer, ConfigFromEnv)
{
  const std::string cacert_str = "--begin and end fake cert--";
  setenv("OTEL_EXPORTER_OTLP_CERTIFICATE_STRING", cacert_str.c_str(), 1);
  setenv("OTEL_EXPORTER_OTLP_SSL_ENABLE", "True", 1);
  const std::string endpoint = "https://localhost:9999";
  setenv("OTEL_EXPORTER_OTLP_ENDPOINT", endpoint.c_str(), 1);
  setenv("OTEL_EXPORTER_OTLP_TIMEOUT", "20050ms", 1);
  setenv("OTEL_EXPORTER_OTLP_HEADERS", "k1=v1,k2=v2", 1);
  setenv("OTEL_EXPORTER_OTLP_TRACES_HEADERS", "k1=v3,k1=v4", 1);

  std::unique_ptr<OtlpGrpcExporter> exporter = std::make_unique<OtlpGrpcExporter>();
  EXPECT_EQ(GetOptions(exporter).ssl_credentials_cacert_as_string, cacert_str);
  EXPECT_EQ(GetOptions(exporter).use_ssl_credentials, true);
  EXPECT_EQ(GetOptions(exporter).endpoint, endpoint);
  EXPECT_EQ(GetOptions(exporter).timeout.count(),
            std::chrono::duration_cast<std::chrono::system_clock::duration>(
                std::chrono::milliseconds{20050})
                .count());
  EXPECT_EQ(GetOptions(exporter).metadata.size(), 3);
  {
    // Test k2
    auto range = GetOptions(exporter).metadata.equal_range("k2");
    EXPECT_TRUE(range.first != range.second);
    EXPECT_EQ(range.first->second, std::string("v2"));
    ++range.first;
    EXPECT_TRUE(range.first == range.second);
  }
  {
    // Test k1
    auto range = GetOptions(exporter).metadata.equal_range("k1");
    EXPECT_TRUE(range.first != range.second);
    EXPECT_EQ(range.first->second, std::string("v3"));
    ++range.first;
    EXPECT_EQ(range.first->second, std::string("v4"));
    ++range.first;
    EXPECT_TRUE(range.first == range.second);
  }

  unsetenv("OTEL_EXPORTER_OTLP_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_CERTIFICATE_STRING");
  unsetenv("OTEL_EXPORTER_OTLP_SSL_ENABLE");
  unsetenv("OTEL_EXPORTER_OTLP_TIMEOUT");
  unsetenv("OTEL_EXPORTER_OTLP_HEADERS");
  unsetenv("OTEL_EXPORTER_OTLP_TRACES_HEADERS");
}

// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpGrpcExporterTestPeer, ConfigHttpsSecureFromEnv)
{
  // https takes precedence over insecure
  const std::string endpoint = "https://localhost:9999";
  setenv("OTEL_EXPORTER_OTLP_ENDPOINT", endpoint.c_str(), 1);
  setenv("OTEL_EXPORTER_OTLP_TRACES_INSECURE", "true", 1);

  std::unique_ptr<OtlpGrpcExporter> exporter = std::make_unique<OtlpGrpcExporter>();
  EXPECT_EQ(GetOptions(exporter).use_ssl_credentials, true);
  EXPECT_EQ(GetOptions(exporter).endpoint, endpoint);

  unsetenv("OTEL_EXPORTER_OTLP_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_TRACES_INSECURE");
}

// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpGrpcExporterTestPeer, ConfigHttpInsecureFromEnv)
{
  // http takes precedence over secure
  const std::string endpoint = "http://localhost:9999";
  setenv("OTEL_EXPORTER_OTLP_ENDPOINT", endpoint.c_str(), 1);
  setenv("OTEL_EXPORTER_OTLP_TRACES_INSECURE", "false", 1);

  std::unique_ptr<OtlpGrpcExporter> exporter = std::make_unique<OtlpGrpcExporter>();
  EXPECT_EQ(GetOptions(exporter).use_ssl_credentials, false);
  EXPECT_EQ(GetOptions(exporter).endpoint, endpoint);

  unsetenv("OTEL_EXPORTER_OTLP_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_TRACES_INSECURE");
}

// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpGrpcExporterTestPeer, ConfigUnknownSecureFromEnv)
{
  const std::string endpoint = "localhost:9999";
  setenv("OTEL_EXPORTER_OTLP_ENDPOINT", endpoint.c_str(), 1);
  setenv("OTEL_EXPORTER_OTLP_TRACES_INSECURE", "false", 1);

  std::unique_ptr<OtlpGrpcExporter> exporter = std::make_unique<OtlpGrpcExporter>();
  EXPECT_EQ(GetOptions(exporter).use_ssl_credentials, true);
  EXPECT_EQ(GetOptions(exporter).endpoint, endpoint);

  unsetenv("OTEL_EXPORTER_OTLP_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_TRACES_INSECURE");
}

// Test exporter configuration options with use_ssl_credentials
TEST_F(OtlpGrpcExporterTestPeer, ConfigUnknownInsecureFromEnv)
{
  const std::string endpoint = "localhost:9999";
  setenv("OTEL_EXPORTER_OTLP_ENDPOINT", endpoint.c_str(), 1);
  setenv("OTEL_EXPORTER_OTLP_TRACES_INSECURE", "true", 1);

  std::unique_ptr<OtlpGrpcExporter> exporter = std::make_unique<OtlpGrpcExporter>();
  EXPECT_EQ(GetOptions(exporter).use_ssl_credentials, false);
  EXPECT_EQ(GetOptions(exporter).endpoint, endpoint);

  unsetenv("OTEL_EXPORTER_OTLP_ENDPOINT");
  unsetenv("OTEL_EXPORTER_OTLP_TRACES_INSECURE");
}

TEST_F(OtlpGrpcExporterTestPeer, ConfigRetryDefaultValues)
{
  std::unique_ptr<OtlpGrpcExporter> exporter = std::make_unique<OtlpGrpcExporter>();
  const auto options                         = GetOptions(exporter);
  ASSERT_EQ(options.retry_policy_max_attempts, 5);
  ASSERT_FLOAT_EQ(options.retry_policy_initial_backoff.count(), 1.0f);
  ASSERT_FLOAT_EQ(options.retry_policy_max_backoff.count(), 5.0f);
  ASSERT_FLOAT_EQ(options.retry_policy_backoff_multiplier, 1.5f);
}

TEST_F(OtlpGrpcExporterTestPeer, ConfigRetryValuesFromEnv)
{
  setenv("OTEL_CPP_EXPORTER_OTLP_TRACES_RETRY_MAX_ATTEMPTS", "123", 1);
  setenv("OTEL_CPP_EXPORTER_OTLP_TRACES_RETRY_INITIAL_BACKOFF", "4.5", 1);
  setenv("OTEL_CPP_EXPORTER_OTLP_TRACES_RETRY_MAX_BACKOFF", "6.7", 1);
  setenv("OTEL_CPP_EXPORTER_OTLP_TRACES_RETRY_BACKOFF_MULTIPLIER", "8.9", 1);

  std::unique_ptr<OtlpGrpcExporter> exporter = std::make_unique<OtlpGrpcExporter>();
  const auto options                         = GetOptions(exporter);
  ASSERT_EQ(options.retry_policy_max_attempts, 123);
  ASSERT_FLOAT_EQ(options.retry_policy_initial_backoff.count(), 4.5f);
  ASSERT_FLOAT_EQ(options.retry_policy_max_backoff.count(), 6.7f);
  ASSERT_FLOAT_EQ(options.retry_policy_backoff_multiplier, 8.9f);

  unsetenv("OTEL_CPP_EXPORTER_OTLP_TRACES_RETRY_MAX_ATTEMPTS");
  unsetenv("OTEL_CPP_EXPORTER_OTLP_TRACES_RETRY_INITIAL_BACKOFF");
  unsetenv("OTEL_CPP_EXPORTER_OTLP_TRACES_RETRY_MAX_BACKOFF");
  unsetenv("OTEL_CPP_EXPORTER_OTLP_TRACES_RETRY_BACKOFF_MULTIPLIER");
}

TEST_F(OtlpGrpcExporterTestPeer, ConfigRetryGenericValuesFromEnv)
{
  setenv("OTEL_CPP_EXPORTER_OTLP_RETRY_MAX_ATTEMPTS", "321", 1);
  setenv("OTEL_CPP_EXPORTER_OTLP_RETRY_INITIAL_BACKOFF", "5.4", 1);
  setenv("OTEL_CPP_EXPORTER_OTLP_RETRY_MAX_BACKOFF", "7.6", 1);
  setenv("OTEL_CPP_EXPORTER_OTLP_RETRY_BACKOFF_MULTIPLIER", "9.8", 1);

  std::unique_ptr<OtlpGrpcExporter> exporter = std::make_unique<OtlpGrpcExporter>();
  const auto options                         = GetOptions(exporter);
  ASSERT_EQ(options.retry_policy_max_attempts, 321);
  ASSERT_FLOAT_EQ(options.retry_policy_initial_backoff.count(), 5.4f);
  ASSERT_FLOAT_EQ(options.retry_policy_max_backoff.count(), 7.6f);
  ASSERT_FLOAT_EQ(options.retry_policy_backoff_multiplier, 9.8f);

  unsetenv("OTEL_CPP_EXPORTER_OTLP_RETRY_MAX_ATTEMPTS");
  unsetenv("OTEL_CPP_EXPORTER_OTLP_RETRY_INITIAL_BACKOFF");
  unsetenv("OTEL_CPP_EXPORTER_OTLP_RETRY_MAX_BACKOFF");
  unsetenv("OTEL_CPP_EXPORTER_OTLP_RETRY_BACKOFF_MULTIPLIER");
}
#endif  // NO_GETENV

#ifdef ENABLE_OTLP_RETRY_PREVIEW
struct TestTraceService : public opentelemetry::proto::collector::trace::v1::TraceService::Service
{
  TestTraceService(const std::vector<grpc::StatusCode> &status_codes) : status_codes_(status_codes)
  {}

  inline grpc::Status Export(
      grpc::ServerContext *,
      const opentelemetry::proto::collector::trace::v1::ExportTraceServiceRequest *,
      opentelemetry::proto::collector::trace::v1::ExportTraceServiceResponse *) override
  {
    ++request_count_;
    return grpc::Status(status_codes_.at(index_++ % status_codes_.size()), "TEST!");
  }

  size_t request_count_ = 0UL;
  size_t index_         = 0UL;
  std::vector<grpc::StatusCode> status_codes_;
};

using StatusCodeVector = std::vector<grpc::StatusCode>;

class OtlpGrpcExporterRetryIntegrationTests
    : public ::testing::TestWithParam<std::tuple<bool, StatusCodeVector, std::size_t>>
{};

INSTANTIATE_TEST_SUITE_P(
    StatusCodes,
    OtlpGrpcExporterRetryIntegrationTests,
    testing::Values(
        // With retry policy enabled
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::CANCELLED}, 5),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::UNKNOWN}, 1),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::INVALID_ARGUMENT}, 1),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::DEADLINE_EXCEEDED}, 5),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::NOT_FOUND}, 1),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::ALREADY_EXISTS}, 1),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::PERMISSION_DENIED}, 1),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::UNAUTHENTICATED}, 1),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::RESOURCE_EXHAUSTED}, 1),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::FAILED_PRECONDITION}, 1),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::ABORTED}, 5),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::OUT_OF_RANGE}, 5),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::UNIMPLEMENTED}, 1),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::INTERNAL}, 1),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::UNAVAILABLE}, 5),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::DATA_LOSS}, 5),
        std::make_tuple(true, StatusCodeVector{grpc::StatusCode::OK}, 1),
        std::make_tuple(true,
                        StatusCodeVector{grpc::StatusCode::UNAVAILABLE, grpc::StatusCode::ABORTED,
                                         grpc::StatusCode::OUT_OF_RANGE,
                                         grpc::StatusCode::DATA_LOSS},
                        5),
        std::make_tuple(true,
                        StatusCodeVector{grpc::StatusCode::UNAVAILABLE,
                                         grpc::StatusCode::UNAVAILABLE,
                                         grpc::StatusCode::UNAVAILABLE, grpc::StatusCode::OK},
                        4),
        std::make_tuple(true,
                        StatusCodeVector{grpc::StatusCode::UNAVAILABLE, grpc::StatusCode::CANCELLED,
                                         grpc::StatusCode::DEADLINE_EXCEEDED, grpc::StatusCode::OK},
                        4),
        // With retry policy disabled
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::CANCELLED}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::UNKNOWN}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::INVALID_ARGUMENT}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::DEADLINE_EXCEEDED}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::NOT_FOUND}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::ALREADY_EXISTS}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::PERMISSION_DENIED}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::UNAUTHENTICATED}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::RESOURCE_EXHAUSTED}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::FAILED_PRECONDITION}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::ABORTED}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::OUT_OF_RANGE}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::UNIMPLEMENTED}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::INTERNAL}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::UNAVAILABLE}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::DATA_LOSS}, 1),
        std::make_tuple(false, StatusCodeVector{grpc::StatusCode::OK}, 1),
        std::make_tuple(false,
                        StatusCodeVector{grpc::StatusCode::UNAVAILABLE, grpc::StatusCode::ABORTED,
                                         grpc::StatusCode::OUT_OF_RANGE,
                                         grpc::StatusCode::DATA_LOSS},
                        1),
        std::make_tuple(false,
                        StatusCodeVector{grpc::StatusCode::UNAVAILABLE,
                                         grpc::StatusCode::UNAVAILABLE,
                                         grpc::StatusCode::UNAVAILABLE, grpc::StatusCode::OK},
                        1),
        std::make_tuple(false,
                        StatusCodeVector{grpc::StatusCode::UNAVAILABLE, grpc::StatusCode::CANCELLED,
                                         grpc::StatusCode::DEADLINE_EXCEEDED, grpc::StatusCode::OK},
                        1)));

TEST_P(OtlpGrpcExporterRetryIntegrationTests, StatusCodes)
{
  namespace otlp      = opentelemetry::exporter::otlp;
  namespace trace_sdk = opentelemetry::sdk::trace;

  const auto is_retry_enabled  = std::get<0>(GetParam());
  const auto status_codes      = std::get<1>(GetParam());
  const auto expected_attempts = std::get<2>(GetParam());
  TestTraceService service(status_codes);
  std::unique_ptr<grpc::Server> server;

  std::promise<void> server_ready;
  auto server_ready_future = server_ready.get_future();

  std::thread server_thread([&server, &service, &server_ready]() {
    std::string address("localhost:4317");
    grpc::ServerBuilder builder;
    builder.RegisterService(&service);
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    server = builder.BuildAndStart();
    server_ready.set_value();
    server->Wait();
  });

  server_ready_future.wait();

  otlp::OtlpGrpcExporterOptions opts{};

  if (is_retry_enabled)
  {
    opts.retry_policy_max_attempts       = 5;
    opts.retry_policy_initial_backoff    = std::chrono::duration<float>{0.1f};
    opts.retry_policy_max_backoff        = std::chrono::duration<float>{5.0f};
    opts.retry_policy_backoff_multiplier = 1.0f;
  }
  else
  {
    opts.retry_policy_max_attempts       = 0;
    opts.retry_policy_initial_backoff    = std::chrono::duration<float>::zero();
    opts.retry_policy_max_backoff        = std::chrono::duration<float>::zero();
    opts.retry_policy_backoff_multiplier = 0.0f;
  }

  auto exporter  = otlp::OtlpGrpcExporterFactory::Create(opts);
  auto processor = trace_sdk::SimpleSpanProcessorFactory::Create(std::move(exporter));
  auto provider  = trace_sdk::TracerProviderFactory::Create(std::move(processor));
  provider->GetTracer("Test tracer")->StartSpan("Test span")->End();
  provider->ForceFlush();

  ASSERT_TRUE(server);
  server->Shutdown();

  if (server_thread.joinable())
  {
    server_thread.join();
  }

  ASSERT_EQ(expected_attempts, service.request_count_);
}
#endif  // ENABLE_OTLP_RETRY_PREVIEW

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
