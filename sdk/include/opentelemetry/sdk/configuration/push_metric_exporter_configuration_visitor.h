// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace configuration
{

class OtlpHttpPushMetricExporterConfiguration;
class OtlpGrpcPushMetricExporterConfiguration;
class ConsolePushMetricExporterConfiguration;
class ExtensionPushMetricExporterConfiguration;

class PushMetricExporterConfigurationVisitor
{
public:
  PushMetricExporterConfigurationVisitor()          = default;
  virtual ~PushMetricExporterConfigurationVisitor() = default;

  virtual void VisitOtlpHttp(const OtlpHttpPushMetricExporterConfiguration *model)   = 0;
  virtual void VisitOtlpGrpc(const OtlpGrpcPushMetricExporterConfiguration *model)   = 0;
  virtual void VisitConsole(const ConsolePushMetricExporterConfiguration *model)     = 0;
  virtual void VisitExtension(const ExtensionPushMetricExporterConfiguration *model) = 0;
};

}  // namespace configuration
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
