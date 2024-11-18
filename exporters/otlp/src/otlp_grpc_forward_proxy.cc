// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <list>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "opentelemetry/version.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_forward_proxy.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_client.h"
#include "opentelemetry/proto/collector/metrics/v1/metrics_service.pb.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"
#include "opentelemetry/proto/collector/logs/v1/logs_service.pb.h"
#include "opentelemetry/sdk/common/global_log_handler.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

using namespace opentelemetry;
using namespace opentelemetry::exporter::otlp;
using namespace opentelemetry::proto::collector::metrics::v1;
using namespace opentelemetry::proto::collector::logs::v1;
using namespace opentelemetry::proto::collector::trace::v1;

struct OtlpGrpcForwardProxy::Impl
{
    bool active{ false };
#if defined(ENABLE_ASYNC_EXPORT) && ENABLE_ASYNC_EXPORT
    bool async{ false };
#endif

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

    OtlpGrpcExporterOptions traceExporterOptions;
    OtlpGrpcMetricExporterOptions metricExporterOptions;
    OtlpGrpcLogRecordExporterOptions logExporterOptions;

    std::unique_ptr<TraceService::StubInterface> traceExporterStub;
    std::unique_ptr<MetricsService::StubInterface> metricExporterStub;
    std::unique_ptr<LogsService::StubInterface> logExporterStub;

    struct TraceExporterProxy;
    struct MetricExporterProxy;
    struct LogExporterProxy;

    std::unique_ptr<TraceExporterProxy> traceExporterProxy;
    std::unique_ptr<MetricExporterProxy> metricExporterProxy;
    std::unique_ptr<LogExporterProxy> logExporterProxy;

    Impl();
    bool HandleExportResult(sdk::common::ExportResult result) const;
    grpc::ServerUnaryReactor* Finish(grpc::CallbackServerContext *cbServerContext, opentelemetry::sdk::common::ExportResult exportResult) const;
};

OtlpGrpcForwardProxy::Impl::Impl()
{
    arenaOptions.initial_block_size = 1024;
    arenaOptions.max_block_size = 1024 * 1024;
}

bool OtlpGrpcForwardProxy::Impl::HandleExportResult(sdk::common::ExportResult result) const
{
    if( active )
    {
        if (result != opentelemetry::sdk::common::ExportResult::kSuccess)
        {
            OTEL_INTERNAL_LOG_ERROR("[otlp_grpc_forward_proxy] AsyncExport failed");// << result);
        }
        else
        {
            OTEL_INTERNAL_LOG_DEBUG("[otlp_grpc_forward_proxy] AsyncExport succeeded!");
        }
    }
    return true;
}

grpc::ServerUnaryReactor* OtlpGrpcForwardProxy::Impl::Finish(grpc::CallbackServerContext *cbServerContext, opentelemetry::sdk::common::ExportResult exportResult) const
{
    grpc::Status grpcStatus;
    if( active )
    {        
        switch( exportResult )
        {
            // Batch was exported successfully.
            case sdk::common::ExportResult::kSuccess:
                grpcStatus = grpc::Status(grpc::StatusCode::OK, "ok");
                break;

            // Batch exporting failed, caller must not retry exporting the same batch and the batch must be dropped.
            case sdk::common::ExportResult::kFailure:
                grpcStatus = grpc::Status(grpc::StatusCode::ABORTED, "ExportResult::kFailure");
                break;

            // The collection does not have enough space to receive the export batch.
            case sdk::common::ExportResult::kFailureFull:
                grpcStatus = grpc::Status(grpc::StatusCode::UNAVAILABLE, "ExportResult::kFailureFull");
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
    }
    else
    {
        grpcStatus = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "OtlpGrpcForwardProxy is not active.", std::to_string(int(exportResult)));
    }
    const auto reactor{ cbServerContext->DefaultReactor() };
    reactor->Finish(grpcStatus);
    return reactor;
}

// This is bit ugly, but still easier to understand (at least by me) than template<>'s
#if defined(ENABLE_ASYNC_EXPORT) && ENABLE_ASYNC_EXPORT
#define MAKE_PROXY(NAME, OPTIONS, STUB, SERVICE, REQUEST, RESPONSE, TEXT) \
    struct OtlpGrpcForwardProxy::Impl::NAME : public SERVICE::CallbackService { \
        OtlpGrpcForwardProxy::Impl& impl; \
        explicit NAME(OtlpGrpcForwardProxy::Impl& impl_): impl(impl_){} \
        grpc::ServerUnaryReactor* Export(grpc::CallbackServerContext* cbServerContext, const REQUEST* req, RESPONSE* resp) override { \
            auto exportResult{ sdk::common::ExportResult::kFailure }; \
            if( impl.active ) { \
                auto context{ impl.client.MakeClientContext(impl.OPTIONS) }; \
                if( impl.async ) { \
                    OTEL_INTERNAL_LOG_DEBUG("[otlp_grpc_forward_proxy] Async " TEXT " export"); \
                    auto arena{ std::make_unique<google::protobuf::Arena>(impl.arenaOptions) }; \
                    auto request{ *req }; \
                    exportResult = impl.client.DelegateAsyncExport( impl.OPTIONS, impl.STUB.get(), std::move(context), std::move(arena), std::move(request), \
                        [implPtr = &impl] (sdk::common::ExportResult r, std::unique_ptr<google::protobuf::Arena>&&, const REQUEST&, RESPONSE* ) -> bool {  \
                            return implPtr->HandleExportResult(r); \
                        }); \
                }  else { \
                    OTEL_INTERNAL_LOG_DEBUG("[otlp_grpc_forward_proxy] Sync " TEXT " export"); \
                    const auto status{ impl.STUB->Export( context.get(), *req, resp ) }; \
                    exportResult = status.ok() ? sdk::common::ExportResult::kSuccess : sdk::common::ExportResult::kFailure; \
                } \
            } \
            return impl.Finish(cbServerContext, exportResult); \
        }};
#else
#define MAKE_PROXY(NAME, OPTIONS, REQUEST, RESPONSE, TEXT) \
    struct OtlpGrpcForwardProxy::Impl::NAME : public SERVICE::CallbackService { \
        OtlpGrpcForwardProxy::Impl& impl; \
        explicit NAME(OtlpGrpcForwardProxy::Impl& impl_): impl(impl_){} \
        grpc::ServerUnaryReactor* Export(grpc::CallbackServerContext* cbServerContext, const REQUEST* req, RESPONSE* resp) override { \
            auto exportResult{ sdk::common::ExportResult::kFailure }; \
            if( impl.active ) { \
                auto context{ impl.client.MakeClientContext(impl.OPTIONS) }; \
                OTEL_INTERNAL_LOG_DEBUG("[otlp_grpc_forward_proxy] Sync " TEXT " export"); \
                const auto status{ impl.STUB->Export( context.get(), *req, resp ) }; \
                exportResult = status.ok() ? sdk::common::ExportResult::kSuccess : sdk::common::ExportResult::kFailure; \
            } \
            return impl.Finish(cbServerContext, exportResult); \
        }};
#endif

MAKE_PROXY(MetricExporterProxy, metricExporterOptions, metricExporterStub, MetricsService, ExportMetricsServiceRequest, ExportMetricsServiceResponse, "metrics")
MAKE_PROXY(TraceExporterProxy, traceExporterOptions, traceExporterStub, TraceService, ExportTraceServiceRequest, ExportTraceServiceResponse, "traces")
MAKE_PROXY(LogExporterProxy, logExporterOptions, logExporterStub, LogsService, ExportLogsServiceRequest, ExportLogsServiceResponse, "logs")

OtlpGrpcForwardProxy::OtlpGrpcForwardProxy()
    : impl(std::make_unique<Impl>())
{
}

OtlpGrpcForwardProxy::~OtlpGrpcForwardProxy()
{
}

void OtlpGrpcForwardProxy::SetActive(bool active_)
{
    assert(impl != nullptr);
    impl->active = active_;
}

bool OtlpGrpcForwardProxy::IsActive() const
{
    assert(impl != nullptr);
    return impl->active;
}

void OtlpGrpcForwardProxy::SetAsync(bool async_)
{
    assert(impl != nullptr);
#if defined(ENABLE_ASYNC_EXPORT) && ENABLE_ASYNC_EXPORT
    impl->async = async_;
#endif
}

bool OtlpGrpcForwardProxy::IsAsync() const
{
    assert(impl != nullptr);
#if defined(ENABLE_ASYNC_EXPORT) && ENABLE_ASYNC_EXPORT
    return impl->async;
#else
    return false;
#endif
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

void OtlpGrpcForwardProxy::RegisterTraceExporter(const OtlpGrpcExporterOptions& options)
{
    assert(impl != nullptr);
    assert(impl->traceExporterProxy == nullptr);
    assert(impl->traceExporterStub == nullptr);
    impl->traceExporterOptions = options;
    impl->traceExporterStub = impl->client.MakeTraceServiceStub(options);
    impl->traceExporterProxy = std::make_unique<Impl::TraceExporterProxy>(*impl);
    impl->serverBuilder.RegisterService(impl->traceExporterProxy.get());
}

void OtlpGrpcForwardProxy::RegisterLogRecordExporter(const OtlpGrpcLogRecordExporterOptions& options)
{
    assert(impl != nullptr);
    assert(impl->logExporterProxy == nullptr);
    assert(impl->logExporterStub == nullptr);
    impl->logExporterOptions = options;
    impl->logExporterStub = impl->client.MakeLogsServiceStub(options);
    impl->logExporterProxy = std::make_unique<Impl::LogExporterProxy>(*impl);
    impl->serverBuilder.RegisterService(impl->logExporterProxy.get());
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

void OtlpGrpcForwardProxy::Shutdown()
{
    assert(impl->server != nullptr);
    impl->server->Shutdown();
}

void EnableDefaultHealthCheckService(bool enable)
{
    grpc::EnableDefaultHealthCheckService(enable);  
}

void InitProtoReflectionServerBuilderPlugin()
{
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
}

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
