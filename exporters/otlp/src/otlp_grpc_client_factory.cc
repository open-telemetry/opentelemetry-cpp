// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/exporters/otlp/otlp_grpc_client_factory.h"
#include <memory>

#include "opentelemetry/exporters/otlp/otlp_grpc_client.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

nostd::shared_ptr<OtlpGrpcClient> OtlpGrpcClientFactory::Create(
    const OtlpGrpcClientOptions &options)
{
  return nostd::shared_ptr<OtlpGrpcClient>(new OtlpGrpcClient(options));
}

nostd::shared_ptr<OtlpGrpcClientReferenceGuard> OtlpGrpcClientFactory::CreateReferenceGuard()
{
  return nostd::shared_ptr<OtlpGrpcClientReferenceGuard>(new OtlpGrpcClientReferenceGuard());
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
