// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "opentelemetry/version.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h"

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
    explicit OtlpGrpcForwardProxy(bool syncExport = false);
    ~OtlpGrpcForwardProxy();
    void AddListenAddress(const std::string& listenAddress);
    void RegisterMetricExporter(const OtlpGrpcMetricExporterOptions& options);
    void Start();
    void Wait();
};

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
