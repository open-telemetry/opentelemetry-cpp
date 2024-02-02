// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class OtlpSpanExporterConfiguration;
class ConsoleSpanExporterConfiguration;
class ZipkinSpanExporterConfiguration;
class ExtensionSpanExporterConfiguration;

class SpanExporterConfigurationVisitor
{
public:
  virtual void VisitOtlp(const OtlpSpanExporterConfiguration *model)           = 0;
  virtual void VisitConsole(const ConsoleSpanExporterConfiguration *model)       = 0;
  virtual void VisitZipkin(const ZipkinSpanExporterConfiguration *model)       = 0;
  virtual void VisitExtension(const ExtensionSpanExporterConfiguration *model) = 0;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
