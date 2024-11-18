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
public:
    explicit OtlpGrpcForwardProxy();
    ~OtlpGrpcForwardProxy();
    void SetActive(bool active);
    bool IsActive() const;
    void SetAsync(bool async);
    bool IsAsync() const;
    void AddListenAddress(const std::string& listenAddress);
    void RegisterTraceExporter(const OtlpGrpcExporterOptions& options);
    void RegisterMetricExporter(const OtlpGrpcMetricExporterOptions& options);
    void RegisterLogRecordExporter(const OtlpGrpcLogRecordExporterOptions& options);
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
