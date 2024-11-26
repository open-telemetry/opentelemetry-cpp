// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_options.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

/**
 * Class to instantiate an otlp grpc forwarding proxy.
 *
 */
class OPENTELEMETRY_EXPORT_TYPE OtlpGrpcForwardProxy
{
    struct Impl;
    std::unique_ptr<Impl> impl;

    OtlpGrpcForwardProxy() = delete;

public:

    explicit OtlpGrpcForwardProxy(const OtlpGrpcClientOptions& options);
    ~OtlpGrpcForwardProxy();

    void SetActive(bool active);
    bool IsActive() const;

    void AddListenAddress(const std::string& listenAddress);

    void RegisterTraceExporter( );
    void RegisterMetricExporter( );
    void RegisterLogRecordExporter( );

    void Start();
    void Wait();
    void Shutdown();

    // These are global gRPC changes.
    static void EnableDefaultHealthCheckService(bool enable);
    static void InitProtoReflectionServerBuilderPlugin();
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
