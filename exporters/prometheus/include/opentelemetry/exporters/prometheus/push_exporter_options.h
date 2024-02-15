// Copyright 2023, OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace metrics
{

/**
 * Struct to hold Prometheus exporter options.
 */
struct PrometheusPushExporterOptions
{
  std::string host;
  std::string port;
  std::string jobname;
  std::unordered_map<std::string, std::string> labels;
  std::string username;
  std::string password;

  std::size_t max_collection_size = 2000;

  inline PrometheusPushExporterOptions() noexcept {}
};

}  // namespace metrics
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
