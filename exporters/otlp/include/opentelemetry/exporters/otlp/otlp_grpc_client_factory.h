// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/exporters/otlp/otlp_grpc_client.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_client_options.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * Factory class for OtlpGrpcClient.
 */
class OPENTELEMETRY_EXPORT OtlpGrpcClientFactory
{
public:
  /**
   * Create an OtlpGrpcClient using all default options.
   */
  static std::shared_ptr<OtlpGrpcClient> Create(const OtlpGrpcClientOptions &options);

  static std::shared_ptr<OtlpGrpcClientReferenceGuard> CreateReferenceGuard();
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
