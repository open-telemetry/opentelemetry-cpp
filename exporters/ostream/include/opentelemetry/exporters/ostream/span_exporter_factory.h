// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <iostream>
#include <memory>

#include "opentelemetry/sdk/version/version.h"
#include "opentelemetry/sdk/trace/exporter.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace trace
{

/**
 * Factory class for OStreamSpanExporter.
 */
class OPENTELEMETRY_API OStreamSpanExporterFactory
{
public:
  /**
   * Creates an OStreamSpanExporter writing to the default location.
   */
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Create();

  /**
   * Creates an OStreamSpanExporter writing to the given location.
   */
  static std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> Create(std::ostream &sout);
};

}  // namespace trace
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
