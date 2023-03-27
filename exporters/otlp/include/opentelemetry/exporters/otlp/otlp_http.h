// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

enum class JsonBytesMappingKind : int
{
  kHexId,
  kHex,
  kBase64,
};

enum class HttpRequestContentType : int
{
  kJson,
  kBinary,
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
