// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <list>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "opentelemetry/version.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_forward_proxy.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_client.h"
#include "opentelemetry/proto/collector/metrics/v1/metrics_service.pb.h"
#include "opentelemetry/sdk/common/global_log_handler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

using namespace opentelemetry;
using namespace opentelemetry::exporter::otlp;
using namespace opentelemetry::proto::collector::metrics::v1;

struct OtlpGrpcForwardProxy::Impl
{
    struct MetricExporterProxy;

    grpc::ServerBuilder serverBuilder;
    std::unique_ptr<grpc::Server> server;
    OtlpGrpcClient client;
    google::protobuf::ArenaOptions arenaOptions;

    struct AddressPortPair
    {
        explicit AddressPortPair(const std::string& address_, int port_ = -1)
            : address(address_), port(port_) {}
        std::string address;
        int port{-1};
    };

    std::list<AddressPortPair> listenAddresses;

    OtlpGrpcMetricExporterOptions metricExporterOptions;
    std::unique_ptr<MetricsService::StubInterface> metricExporterStub;
    std::unique_ptr<MetricExporterProxy> metricExporterProxy;

    bool syncExport{ false };

    Impl(bool syncExport);
    bool HandleExportResult(sdk::common::ExportResult result) const;
    grpc::ServerUnaryReactor* Finish(grpc::CallbackServerContext *cbServerContext, opentelemetry::sdk::common::ExportResult exportResult) const;
};

OtlpGrpcForwardProxy::Impl::Impl(bool syncExport_)
    : syncExport(syncExport_)
{
    arenaOptions.initial_block_size = 1024;
    arenaOptions.max_block_size = 1024 * 1024;
}

bool OtlpGrpcForwardProxy::Impl::HandleExportResult(sdk::common::ExportResult result) const
{
    if (result != opentelemetry::sdk::common::ExportResult::kSuccess)
    {
        OTEL_INTERNAL_LOG_ERROR("[OTLP GRPC FORWARD PROXY] AsyncExport failed with result ");// << result);
    }
    else
    {
        OTEL_INTERNAL_LOG_DEBUG("[OTLP GRPC FORWARD PROXY] AsyncExport succeeded!");
    }
    return true;
}

grpc::ServerUnaryReactor* OtlpGrpcForwardProxy::Impl::Finish(grpc::CallbackServerContext *cbServerContext, opentelemetry::sdk::common::ExportResult exportResult) const
{
    grpc::Status grpcStatus;
    switch( exportResult )
    {
        // Batch was exported successfully.
        case sdk::common::ExportResult::kSuccess:
            grpcStatus = grpc::Status(grpc::StatusCode::OK, "ok");
            break;

        // Batch exporting failed, caller must not retry exporting the same batch and the batch must be dropped.
        case sdk::common::ExportResult::kFailure:
            grpcStatus = grpc::Status(grpc::StatusCode::UNAVAILABLE, "ExportResult::kFailure");
            break;

        // The collection does not have enough space to receive the export batch.
        case sdk::common::ExportResult::kFailureFull:
            grpcStatus = grpc::Status(grpc::StatusCode::RESOURCE_EXHAUSTED, "ExportResult::kFailureFull");
            break;

        case sdk::common::ExportResult::kFailureInvalidArgument:
            grpcStatus = grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "ExportResult::kFailureInvalidArgument");
            break;

        default:
            grpcStatus = grpc::Status(grpc::StatusCode::UNKNOWN, "ExportResult::???", std::to_string(int(exportResult)));
            break;
    }

    if( exportResult != sdk::common::ExportResult::kSuccess )
    {
        OTEL_INTERNAL_LOG_ERROR("[OTLP GRPC FORWARD PROXY] Finish failed with result "); // << exportResult << " and status code " << grpcStatus);
    }

    const auto reactor{ cbServerContext->DefaultReactor() };
    reactor->Finish(grpcStatus);
    return reactor;
}

struct OtlpGrpcForwardProxy::Impl::MetricExporterProxy : public MetricsService::CallbackService 
{
    const OtlpGrpcForwardProxy::Impl& impl;
    explicit MetricExporterProxy(const OtlpGrpcForwardProxy::Impl& impl_): impl(impl_){}
    grpc::ServerUnaryReactor* Export(grpc::CallbackServerContext* cbServerContext, const ExportMetricsServiceRequest* req, ExportMetricsServiceResponse* resp) override
    {
        auto context{ impl.client.MakeClientContext(impl.metricExporterOptions) };
        auto exportResult{ sdk::common::ExportResult::kFailure };
        bool exported{ false };
#if defined(ENABLE_ASYNC_EXPORTER) && ENABLE_ASYNC_EXPORTER
        if( !syncExport )
        {
            auto arena{ std::make_unique<google::protobuf::Arena>(impl.arenaOptions) };
            auto request{ ExportMetricsServiceRequest->New(arena) };
            request->CopyFrom( *req );
            exportResult = impl.client.DelegateAsyncExport( impl.metricExporterOptions, impl.metricExporterStub.get(), std::move(context), std::move(arena), std::move(request),
                [impl = &impl] (sdk::common::ExportResult r, std::unique_ptr<google::protobuf::Arena>&&, const ExportMetricsServiceRequest&, ExportMetricsServiceResponse* ) -> bool 
                { 
                    return impl->HandleExportResult(r); 
                });
            exported = true;
        }
#endif
        if( !exported )
        {
            const auto status{ impl.metricExporterStub->Export( context.get(), *req, resp ) };
            exportResult = status.ok() ? sdk::common::ExportResult::kSuccess : sdk::common::ExportResult::kFailure;
        }
        return impl.Finish(cbServerContext, exportResult);
    }
};

OtlpGrpcForwardProxy::OtlpGrpcForwardProxy(bool syncExport)
    : impl(std::make_unique<Impl>(syncExport))
{
}

void OtlpGrpcForwardProxy::AddListenAddress(const std::string& listenAddress)
{
    assert(impl != nullptr);
    auto& pair = impl->listenAddresses.emplace_back(listenAddress, -1);
    impl->serverBuilder.AddListeningPort(listenAddress, grpc::InsecureServerCredentials(), &( pair.port ));
}

void OtlpGrpcForwardProxy::RegisterMetricExporter(const OtlpGrpcMetricExporterOptions& options)
{
    assert(impl != nullptr);
    assert(impl->metricExporterProxy == nullptr);
    assert(impl->metricExporterStub == nullptr);
    impl->metricExporterOptions = options;
    impl->metricExporterStub = impl->client.MakeMetricsServiceStub(options);
    impl->metricExporterProxy = std::make_unique<Impl::MetricExporterProxy>(*impl);
    impl->serverBuilder.RegisterService(impl->metricExporterProxy.get());
}

void OtlpGrpcForwardProxy::Start()
{
    assert(impl->server == nullptr);
    impl->server = impl->serverBuilder.BuildAndStart();
    assert(impl->server != nullptr);
}

void OtlpGrpcForwardProxy::Wait()
{
    assert(impl->server != nullptr);
    impl->server->Wait();
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
