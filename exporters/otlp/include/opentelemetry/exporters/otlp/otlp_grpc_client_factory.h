// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/exporters/otlp/otlp_grpc_client_options.h"
#include "opentelemetry/nostd/shared_ptr.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

#ifdef ENABLE_ASYNC_EXPORT
class OtlpGrpcClientReferenceGuard;
#endif
class OtlpGrpcClient;

/**
 * Factory class for OtlpGrpcClient.
 */
class OPENTELEMETRY_EXPORT OtlpGrpcClientFactory
{
public:
  /**
   * Create an OtlpGrpcClient using all default options.
   */
  static nostd::shared_ptr<OtlpGrpcClient> Create(const OtlpGrpcClientOptions &options);

#ifdef ENABLE_ASYNC_EXPORT
  static nostd::shared_ptr<OtlpGrpcClientReferenceGuard> CreateReferenceGuard();
#endif
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
