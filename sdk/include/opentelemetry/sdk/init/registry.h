// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace init
{
class OtlpSpanExporterBuilder;
class ConsoleSpanExporterBuilder;
class ZipkinSpanExporterBuilder;

class Registry
{
public:
  static const OtlpSpanExporterBuilder *GetOtlpBuilder();
  static void SetOtlpBuilder(const OtlpSpanExporterBuilder * builder);

  static const ConsoleSpanExporterBuilder *GetConsoleBuilder();
  static void SetConsoleBuilder(const ConsoleSpanExporterBuilder * builder);

  static const ZipkinSpanExporterBuilder *GetZipkinBuilder();
  static void SetZipkinBuilder(const ZipkinSpanExporterBuilder * builder);
};

}  // namespace init
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
