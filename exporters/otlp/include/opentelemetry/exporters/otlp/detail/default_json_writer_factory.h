// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/exporters/otlp/otlp_json_writer_factory.h"
#ifdef ENABLE_JSON_WRITER_NLOHMANN
#  include "opentelemetry/exporters/otlp/otlp_json_writer_factory_nlohmann.h"
#else
#  include "opentelemetry/sdk/common/global_log_handler.h"
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{
namespace detail
{

inline std::shared_ptr<JsonWriterFactory> GetDefaultJsonWriterFactory()
{
#ifdef ENABLE_JSON_WRITER_NLOHMANN
  static auto instance = std::make_shared<JsonWriterFactoryNlohmann>();
  return instance;
#else
  OTEL_INTERNAL_LOG_ERROR(
      "No default JSON writer backend is compiled in. "
      "Use the JsonWriterFactory constructor or factory overloads, "
      "or enable the nlohmann backend (OTELCPP_WITH_JSON_WRITER_NLOHMANN=ON).");
  std::terminate();
#endif
}

}  // namespace detail
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
