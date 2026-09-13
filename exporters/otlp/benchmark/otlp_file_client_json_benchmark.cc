// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <benchmark/benchmark.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

#include "opentelemetry/exporters/otlp/otlp_file_client.h"
#include "opentelemetry/exporters/otlp/otlp_file_client_options.h"
#include "opentelemetry/exporters/otlp/otlp_file_client_runtime_options.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/common/global_log_handler.h"

// clang-format off
#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h" // IWYU pragma: keep
#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"
#include "opentelemetry/proto/common/v1/common.pb.h"
#include "opentelemetry/proto/resource/v1/resource.pb.h"
#include "opentelemetry/proto/trace/v1/trace.pb.h"
#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h" // IWYU pragma: keep
// clang-format on

namespace otlp       = opentelemetry::exporter::otlp;
namespace proto      = opentelemetry::proto;
namespace nostd      = opentelemetry::nostd;
namespace common_log = opentelemetry::sdk::common::internal_log;

namespace
{

// Discards the serialized body, so a run measures the protobuf to JSON conversion and the
// serialization to a string, without file I/O.
class DiscardingAppender : public otlp::OtlpFileAppender
{
public:
  void Export(nostd::string_view data, std::size_t) override
  {
    benchmark::DoNotOptimize(data.data());
  }

  bool ForceFlush(std::chrono::microseconds) noexcept override { return true; }

  bool Shutdown(std::chrono::microseconds) noexcept override { return true; }
};

void AddAttribute(proto::common::v1::KeyValue *attribute, const std::string &key, std::int64_t i)
{
  attribute->set_key(key);
  attribute->mutable_value()->set_int_value(i);
}

void AddAttribute(proto::common::v1::KeyValue *attribute,
                  const std::string &key,
                  const std::string &value)
{
  attribute->set_key(key);
  attribute->mutable_value()->set_string_value(value);
}

// One resource and scope holding n_spans spans, each with a mix of attribute types, an event
// and a parent, so every JSON token kind and the bytes mapping take part.
proto::collector::trace::v1::ExportTraceServiceRequest MakeRequest(std::size_t n_spans)
{
  proto::collector::trace::v1::ExportTraceServiceRequest request;
  auto *resource_spans = request.add_resource_spans();
  AddAttribute(resource_spans->mutable_resource()->add_attributes(), "service.name",
               "otlp-json-benchmark");

  auto *scope_spans = resource_spans->add_scope_spans();
  scope_spans->mutable_scope()->set_name("benchmark");
  scope_spans->mutable_scope()->set_version("1.0.0");

  const std::string trace_id(16, '\x5a');
  for (std::size_t i = 0; i < n_spans; ++i)
  {
    auto *span = scope_spans->add_spans();
    span->set_trace_id(trace_id);
    span->set_span_id(std::string(8, static_cast<char>(i)));
    span->set_parent_span_id(std::string(8, '\x01'));
    span->set_name("GET /api/v1/resource");
    span->set_kind(proto::trace::v1::Span::SPAN_KIND_SERVER);
    span->set_start_time_unix_nano(1700000000000000000ULL + i);
    span->set_end_time_unix_nano(1700000000000100000ULL + i);

    AddAttribute(span->add_attributes(), "http.request.method", "GET");
    AddAttribute(span->add_attributes(), "url.path", "/api/v1/resource");
    AddAttribute(span->add_attributes(), "http.response.status_code", 200);
    auto *ratio = span->add_attributes();
    ratio->set_key("sample.ratio");
    ratio->mutable_value()->set_double_value(0.25);
    auto *retried = span->add_attributes();
    retried->set_key("retried");
    retried->mutable_value()->set_bool_value(false);

    auto *event = span->add_events();
    event->set_name("cache.miss");
    event->set_time_unix_nano(1700000000000050000ULL + i);
    AddAttribute(event->add_attributes(), "cache.key", "resource:" + std::to_string(i));

    span->mutable_status()->set_code(proto::trace::v1::Status::STATUS_CODE_OK);
  }
  return request;
}

void BM_OtlpFileClient_Export_Json(benchmark::State &state)
{
  otlp::OtlpFileClientOptions options;
  options.backend_options = nostd::shared_ptr<otlp::OtlpFileAppender>(new DiscardingAppender());
  otlp::OtlpFileClient client(std::move(options), otlp::OtlpFileClientRuntimeOptions());

  const auto n_spans = static_cast<std::size_t>(state.range(0));
  const auto request = MakeRequest(n_spans);

  for (auto _ : state)
  {
    benchmark::DoNotOptimize(client.Export(request, n_spans));
  }

  state.SetItemsProcessed(state.iterations() * state.range(0));
}

BENCHMARK(BM_OtlpFileClient_Export_Json)
    ->RangeMultiplier(10)
    ->Range(1, 1000)
    ->Unit(benchmark::kMicrosecond);

}  // namespace

int main(int argc, char **argv)
{
  common_log::GlobalLogHandler::SetLogLevel(common_log::LogLevel::None);

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
