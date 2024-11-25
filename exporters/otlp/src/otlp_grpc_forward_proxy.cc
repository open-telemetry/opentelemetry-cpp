// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>
#include <list>
#include <string>
#include <random>

#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "opentelemetry/version.h"

#include "opentelemetry/sdk/common/global_log_handler.h"

#include "opentelemetry/exporters/otlp/otlp_grpc_forward_proxy.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_client.h"
#include "opentelemetry/exporters/otlp/otlp_grpc_client_factory.h"

#include "opentelemetry/proto/collector/metrics/v1/metrics_service.pb.h"
#include "opentelemetry/proto/collector/trace/v1/trace_service.pb.h"
#include "opentelemetry/proto/collector/logs/v1/logs_service.pb.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

// Detect loops, by sending a random identifier for each forward proxy instance.
// All identifiers received are collected, and send as individual http headers.
static std::string kFwProxyRidHeader{ "otel-grpc-fw-proxy-uid" };

using namespace opentelemetry;

using namespace opentelemetry::exporter::otlp;

using namespace opentelemetry::proto::collector::logs::v1;
using namespace opentelemetry::proto::collector::trace::v1;
using namespace opentelemetry::proto::collector::metrics::v1;

struct OtlpGrpcForwardProxy::Impl
{
    bool active{ false };

    OtlpGrpcClientOptions clientOptions;
    std::shared_ptr<OtlpGrpcClient> client;

    grpc::ServerBuilder serverBuilder;
    std::unique_ptr<grpc::Server> server;

    google::protobuf::ArenaOptions arenaOptions;

    struct AddressPortPair
    {
        explicit AddressPortPair(const std::string& address_, int port_ = -1)
            : address(address_), port(port_) {}
        std::string address;
        int port{-1};
    };

    std::list<AddressPortPair> listenAddresses;

    std::unique_ptr<LogsService::StubInterface> logExporterStub;
    std::unique_ptr<TraceService::StubInterface> traceExporterStub;
    std::unique_ptr<MetricsService::StubInterface> metricExporterStub;

#if defined(ENABLE_ASYNC_EXPORT)
    ExportMode logExporterMode{ ExportMode::Unknown };
    ExportMode traceExporterMode{ ExportMode::Unknown };;
    ExportMode metricExporterMode{ ExportMode::Unknown };;

    struct LogExporterProxyAsync;
    struct TraceExporterProxyAsync;
    struct MetricExporterProxyAsync;

    std::unique_ptr<LogExporterProxyAsync> logExporterProxyAsync;
    std::unique_ptr<TraceExporterProxyAsync> traceExporterProxyAsync;
    std::unique_ptr<MetricExporterProxyAsync> metricExporterProxyAsync;
#endif

    struct LogExporterProxy;
    struct TraceExporterProxy;
    struct MetricExporterProxy;

    std::unique_ptr<LogExporterProxy> logExporterProxy;
    std::unique_ptr<TraceExporterProxy> traceExporterProxy;
    std::unique_ptr<MetricExporterProxy> metricExporterProxy;

    std::string fw_proxy_id;

    static inline char ascii_tolower( const char c ) noexcept
    {
        const unsigned char letter = c - 'A';
        return letter < 26 ? ( c | 0x20 ) : c;
    }

    static inline bool ascii_strieq( const char *s0, const char *s1 ) noexcept
    {
        assert(s0);
        assert(s1);
        for( ;; )
        {
            const auto c0{ ascii_tolower( *s0 ++ ) };
            const auto c1{ ascii_tolower( *s1 ++ ) };
            if( c0 != c1 )
                return false;
            if( c0 == 0 ) // reached end, also c0 == c1
                return true;
        }
    }    

    bool CheckForLoop( grpc::ClientContext* context, const std::multimap<grpc::string_ref, grpc::string_ref>& client_metadata ) const
    {
        assert(context != nullptr);
        const auto matching_keys{ client_metadata.equal_range( kFwProxyRidHeader ) };
        auto index{ 0 };
        for( auto it = matching_keys.first; it != matching_keys.second; ++it )
        {
            std::string value(it->second.cbegin(), it->second.cend());
            OTEL_INTERNAL_LOG_DEBUG("[otlp_grpc_forward_proxy] proxy=" << fw_proxy_id << " got=" << value << " index=" << ++index);
            if( ascii_strieq( value.c_str(), fw_proxy_id.c_str() ) )
            {
                OTEL_INTERNAL_LOG_ERROR("[otlp_grpc_forward_proxy] Loop detected!");
                return true;
            }
            else
            {
                // Add what we've got so far
                context->AddMetadata(kFwProxyRidHeader, value );
            }
        }
        // Add ourselves too.
        context->AddMetadata(kFwProxyRidHeader, fw_proxy_id);
        return false;
    }

    Impl(const OtlpGrpcClientOptions&);
#if defined(ENABLE_ASYNC_EXPORT)
    bool HandleExportResult(sdk::common::ExportResult result) const;
    grpc::ServerUnaryReactor* Finish(grpc::CallbackServerContext *cbServerContext, opentelemetry::sdk::common::ExportResult exportResult, const grpc::Status& syncStatus) const;
#endif
};

OtlpGrpcForwardProxy::Impl::Impl(const OtlpGrpcClientOptions& options)
    : clientOptions(options)
{
    arenaOptions.initial_block_size = 65536;
    arenaOptions.max_block_size = 128 * 1024 * 1024;
    client = OtlpGrpcClientFactory::Create(clientOptions);

    char buf[32]{};
    std::random_device rd;
    uint64_t p0 = (uint64_t(rd())<<32) | uint64_t(rd());
    uint64_t p1 = (uint64_t(rd())<<32) | uint64_t(rd());
    sprintf_s(buf, "%08.8lx%08.8lx", p0, p1);
    fw_proxy_id = buf;

}

#if defined(ENABLE_ASYNC_EXPORT)
bool OtlpGrpcForwardProxy::Impl::HandleExportResult(sdk::common::ExportResult result) const
{
    if( active )
    {
        if (result != opentelemetry::sdk::common::ExportResult::kSuccess)
        {
            OTEL_INTERNAL_LOG_ERROR("[otlp_grpc_forward_proxy] AsyncExport failed");
        }
        else
        {
            OTEL_INTERNAL_LOG_DEBUG("[otlp_grpc_forward_proxy] AsyncExport succeeded!");
        }
    }
    return true;
}

grpc::ServerUnaryReactor* OtlpGrpcForwardProxy::Impl::Finish(grpc::CallbackServerContext *cbServerContext, opentelemetry::sdk::common::ExportResult exportResult, const grpc::Status& syncStatus ) const
{
    grpc::Status grpcStatus;
    if( active )
    {        
        if( syncStatus.error_code() != grpc::DO_NOT_USE )
        {
            grpcStatus = syncStatus;
        }
        else
        {
            switch( exportResult )
            {
                // Batch was exported successfully.
                case sdk::common::ExportResult::kSuccess:
                    grpcStatus = grpc::Status(grpc::StatusCode::OK, "");
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
                    grpcStatus = grpc::Status(grpc::StatusCode::UNKNOWN, "ExportResult::???");
                    break;
            }
        }
    }
    else
    {
        grpcStatus = grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "OtlpGrpcForwardProxy is not active.");
    }
    const auto reactor{ cbServerContext->DefaultReactor() };
    reactor->Finish(grpcStatus);
    return reactor;
}
#endif

// This is bit ugly, but still easier to understand (at least by me) than template<>'s
#if defined(ENABLE_ASYNC_EXPORT)
#define MAKE_PROXY_ASYNC(NAME, STUB, SERVICE, REQUEST, RESPONSE, TEXT, MODE) \
    struct OtlpGrpcForwardProxy::Impl::NAME final : public SERVICE::CallbackService { \
        OtlpGrpcForwardProxy::Impl& impl; \
        explicit NAME(OtlpGrpcForwardProxy::Impl& impl_): impl(impl_){} \
        grpc::ServerUnaryReactor* Export(grpc::CallbackServerContext* cbServerContext, const REQUEST* req, RESPONSE* resp) override { \
            auto exportResult{ sdk::common::ExportResult::kFailure }; \
            if( impl.active ) { \
                auto context{ impl.client->MakeClientContext(impl.clientOptions) }; \
                if( impl.CheckForLoop(context.get(), cbServerContext->client_metadata()) ) \
                    return impl.Finish(cbServerContext, exportResult, grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "OtlpGrpcForwardProxy detected loop.")); \
                OTEL_INTERNAL_LOG_DEBUG("[otlp_grpc_forward_proxy] " TEXT " export"); \
                auto syncStatus{ grpc::Status(grpc::Status(grpc::StatusCode::DO_NOT_USE, "")) }; \
                auto arena{ std::make_unique<google::protobuf::Arena>(impl.arenaOptions) }; \
                auto request{ *req }; \
                exportResult = impl.client->DelegateAsyncExport( impl.clientOptions, impl.STUB.get(), std::move(context), std::move(arena), std::move(request), \
                    [implPtr = &impl] (sdk::common::ExportResult r, std::unique_ptr<google::protobuf::Arena>&&, const REQUEST&, RESPONSE* ) -> bool {  \
                        return implPtr->HandleExportResult(r); \
                    }); \
                if( exportResult == sdk::common::ExportResult::kFailureFull && impl.MODE == OtlpGrpcForwardProxy::ExportMode::AsyncBlockOnFull ) { \
                    OTEL_INTERNAL_LOG_DEBUG("[otlp_grpc_forward_proxy] " TEXT " export (blocking)"); \
                    auto syncContext{ impl.client->MakeClientContext(impl.clientOptions ) }; \
                    syncStatus = impl.STUB->Export( syncContext.get(), *req, resp ); \
                } \
                return impl.Finish(cbServerContext, exportResult, syncStatus); \
            } \
            return impl.Finish(cbServerContext, exportResult, grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "OtlpGrpcForwardProxy is not active.")); \
        }};
#endif

#define MAKE_PROXY(NAME, STUB, SERVICE, REQUEST, RESPONSE, TEXT) \
    struct OtlpGrpcForwardProxy::Impl::NAME final : public SERVICE::Service { \
        OtlpGrpcForwardProxy::Impl& impl; \
        explicit NAME(OtlpGrpcForwardProxy::Impl& impl_): impl(impl_){} \
        grpc::Status Export(grpc::ServerContext* serverContext, const REQUEST* req, RESPONSE* resp) override { \
            if( impl.active ) { \
                auto context{ impl.client->MakeClientContext(impl.clientOptions) }; \
                if( impl.CheckForLoop( context.get(), serverContext->client_metadata()) ) \
                    return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "OtlpGrpcForwardProxy detected loop."); \
                OTEL_INTERNAL_LOG_DEBUG("[otlp_grpc_forward_proxy] " TEXT " export"); \
                return impl.STUB->Export( context.get(), *req, resp ); \
            } \
            return grpc::Status(grpc::StatusCode::FAILED_PRECONDITION, "OtlpGrpcForwardProxy is not active."); \
        }};

#if defined(ENABLE_ASYNC_EXPORT)
MAKE_PROXY_ASYNC(MetricExporterProxyAsync, metricExporterStub, MetricsService, ExportMetricsServiceRequest, ExportMetricsServiceResponse, "Async Metric", metricExporterMode)
MAKE_PROXY_ASYNC(TraceExporterProxyAsync, traceExporterStub, TraceService, ExportTraceServiceRequest, ExportTraceServiceResponse, "Async Trace", traceExporterMode)
MAKE_PROXY_ASYNC(LogExporterProxyAsync, logExporterStub, LogsService, ExportLogsServiceRequest, ExportLogsServiceResponse, "Async Log", logExporterMode)
#endif

MAKE_PROXY(MetricExporterProxy, metricExporterStub, MetricsService, ExportMetricsServiceRequest, ExportMetricsServiceResponse, "Metric")
MAKE_PROXY(TraceExporterProxy, traceExporterStub, TraceService, ExportTraceServiceRequest, ExportTraceServiceResponse, "Trace")
MAKE_PROXY(LogExporterProxy, logExporterStub, LogsService, ExportLogsServiceRequest, ExportLogsServiceResponse, "Log")

OtlpGrpcForwardProxy::OtlpGrpcForwardProxy(const OtlpGrpcClientOptions& options_)
    : impl(std::make_unique<Impl>(options_))
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

void OtlpGrpcForwardProxy::AddListenAddress(const std::string& listenAddress)
{
    assert(impl != nullptr);
    auto& pair = impl->listenAddresses.emplace_back(listenAddress, -1);
    impl->serverBuilder.AddListeningPort(listenAddress, grpc::InsecureServerCredentials(), &( pair.port ));
}

void OtlpGrpcForwardProxy::RegisterMetricExporter( ExportMode exportMode )
{
    assert(impl != nullptr);
    assert(impl->metricExporterStub == nullptr);
    assert(impl->metricExporterProxy == nullptr);
    impl->metricExporterStub = impl->client->MakeMetricsServiceStub();
#if defined(ENABLE_ASYNC_EXPORT)
    assert(impl->metricExporterProxyAsync == nullptr);
    assert(impl->metricExporterMode == ExportMode::Unknown);
    if( exportMode != ExportMode::Sync && impl->clientOptions.max_concurrent_requests > 1 )
    {
        impl->metricExporterMode = exportMode;
        impl->metricExporterProxyAsync = std::make_unique<Impl::MetricExporterProxyAsync>(*impl);
        impl->serverBuilder.RegisterService(impl->metricExporterProxyAsync.get());
        return;
    }
    impl->metricExporterMode = ExportMode::Sync;
#endif
    impl->metricExporterProxy = std::make_unique<Impl::MetricExporterProxy>(*impl);
    impl->serverBuilder.RegisterService(impl->metricExporterProxy.get());
}

void OtlpGrpcForwardProxy::RegisterTraceExporter( ExportMode exportMode )
{
    assert(impl != nullptr);
    assert(impl->traceExporterStub == nullptr);
    assert(impl->traceExporterProxy == nullptr);
    impl->traceExporterStub = impl->client->MakeTraceServiceStub();
#if defined(ENABLE_ASYNC_EXPORT)
    assert(impl->traceExporterProxyAsync == nullptr);
    assert(impl->traceExporterMode == ExportMode::Unknown);
    if( exportMode != ExportMode::Sync && impl->clientOptions.max_concurrent_requests > 1 )
    {
        impl->traceExporterMode = exportMode;
        impl->traceExporterProxyAsync = std::make_unique<Impl::TraceExporterProxyAsync>(*impl);
        impl->serverBuilder.RegisterService(impl->traceExporterProxyAsync.get());
        return;
    }
    impl->traceExporterMode = ExportMode::Sync;
#endif
    impl->traceExporterProxy = std::make_unique<Impl::TraceExporterProxy>(*impl);
    impl->serverBuilder.RegisterService(impl->traceExporterProxy.get());
}

void OtlpGrpcForwardProxy::RegisterLogRecordExporter( ExportMode exportMode )
{
    assert(impl != nullptr);
    assert(impl->logExporterStub == nullptr);
    assert(impl->logExporterProxy == nullptr);
    impl->logExporterStub = impl->client->MakeLogsServiceStub();
#if defined(ENABLE_ASYNC_EXPORT)
    assert(impl->logExporterProxyAsync == nullptr);
    assert(impl->logExporterMode == ExportMode::Unknown);
    if( exportMode != ExportMode::Sync && impl->clientOptions.max_concurrent_requests > 1 )
    {
        impl->logExporterMode = exportMode;
        impl->logExporterProxyAsync = std::make_unique<Impl::LogExporterProxyAsync>(*impl);
        impl->serverBuilder.RegisterService(impl->logExporterProxyAsync.get());
        return;
    }
    impl->logExporterMode = ExportMode::Sync;
#endif
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
