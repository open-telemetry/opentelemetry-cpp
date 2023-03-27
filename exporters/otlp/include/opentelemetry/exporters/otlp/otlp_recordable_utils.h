// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/nostd/span.h"

#include "opentelemetry/exporters/otlp/protobuf_include_prefix.h"

#include "opentelemetry/proto/collector/logs/v1/logs_service.pb.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"

#include "opentelemetry/exporters/otlp/protobuf_include_suffix.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace logs
{
class Recordable;
}  // namespace logs
namespace trace
{
class Recordable;
}  // namespace trace
}  // namespace sdk

namespace exporter
{
namespace otlp
{
/**
 * The OtlpRecordableUtils contains utility functions for OTLP recordable
 */
class OtlpRecordableUtils
{
public:
  static void PopulateRequest(
      const nostd::span<std::unique_ptr<opentelemetry::sdk::trace::Recordable>> &spans,
      proto::collector::trace::v1::ExportTraceServiceRequest *request) noexcept;

#ifdef ENABLE_LOGS_PREVIEW
  static void PopulateRequest(
      const nostd::span<std::unique_ptr<opentelemetry::sdk::logs::Recordable>> &logs,
      proto::collector::logs::v1::ExportLogsServiceRequest *request) noexcept;
#endif
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
