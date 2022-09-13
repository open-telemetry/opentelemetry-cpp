// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <grpcpp/grpcpp.h>

#include <memory>

#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * The OTLP gRPC client contains utility functions of gRPC.
 */
class OtlpGrpcClient
{
public:
  /**
   * Create gRPC channel from the exporter options.
   */
  static std::shared_ptr<grpc::Channel> MakeChannel(const OtlpGrpcExporterOptions &options);

  /**
   * Create gRPC client context to call RPC.
   */
  static std::unique_ptr<grpc::ClientContext> MakeClientContext(
      const OtlpGrpcExporterOptions &options);

  /**
   * Create service stub to communicate with the OpenTelemetry Collector.
   */
  template <class ServiceType>
  static std::unique_ptr<typename ServiceType::Stub> MakeServiceStub(
      const OtlpGrpcExporterOptions &options)
  {
    return ServiceType::NewStub(MakeChannel(options));
  }
};
}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
