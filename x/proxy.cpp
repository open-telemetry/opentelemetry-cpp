#include <cstdio>
#include <string>
#include <atomic>
#include <thread>

#include <opentelemetry/exporters/otlp/otlp_grpc_client.h>
#include <opentelemetry/proto/collector/metrics/v1/metrics_service.grpc.pb.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_options.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

using namespace opentelemetry;
using namespace opentelemetry::exporter::otlp;
using namespace opentelemetry::proto::collector::metrics::v1;

class MetricsProxyService : public MetricsService::CallbackService {
public:
  MetricsProxyService()
  {
    metricsOtlpOptions.endpoint = "motel.cat.factz.com:4317";
    arenaOptions.initial_block_size = 4096;
    arenaOptions.max_block_size = 128 * 1024 * 1024;
    otlpStub = otlpClient.MakeMetricsServiceStub(metricsOtlpOptions);
  }

  grpc::ServerUnaryReactor* Export(grpc::CallbackServerContext *cbServerContext, const ExportMetricsServiceRequest* request, ExportMetricsServiceResponse* response) override
  {
    auto arena{ std::make_unique<google::protobuf::Arena>(arenaOptions) };
    auto requestCopy{ *request };
    auto otlpClientContext{ otlpClient.MakeClientContext(metricsOtlpOptions) };

    {
        const auto callCount_{ callCount.fetch_add(1) };
        const auto payloadBytes_{ request->ByteSizeLong() };
        const auto totalBytes_{ totalBytes.fetch_add(payloadBytes_) };
        printf("%s: tid=%zu, cc=%d bytes=%d totalBytes=%zd (%.2f KB, %.2f MB)\n", 
            MetricsService::service_full_name(), 
            std::this_thread::get_id(),
            callCount_, 
            payloadBytes_, 
            totalBytes_, 
            double(totalBytes_) / 1024.0, 
            double(totalBytes_) / (1024.0*1024.0)
        );
    }

    const auto exportResult 
    { 
        otlpClient.DelegateAsyncExport(metricsOtlpOptions, otlpStub.get(), std::move(otlpClientContext), std::move(arena), std::move(requestCopy),
            [](sdk::common::ExportResult result, std::unique_ptr<google::protobuf::Arena> &&, const ExportMetricsServiceRequest &req, ExportMetricsServiceResponse *resp) 
            {
                if (result != opentelemetry::sdk::common::ExportResult::kSuccess)
                {
                    fprintf(stderr, "Error: %d, req_size=%d\n", result, req.resource_metrics_size() );
                    fflush(stdout);
                }
                else
                {
                    fprintf(stdout, "All Good, req_size=%d\n", req.resource_metrics_size() );
                    fflush(stdout);
                }
                return true;
            }
        )
    };

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

    const auto reactor{ cbServerContext->DefaultReactor() };
    reactor->Finish(grpcStatus);
    return reactor;
  }

  std::atomic<int> callCount;
  std::atomic<uint64_t> totalBytes;

  OtlpGrpcMetricExporterOptions metricsOtlpOptions;
  google::protobuf::ArenaOptions arenaOptions;
  OtlpGrpcClient otlpClient;
  std::unique_ptr<MetricsService::StubInterface> otlpStub;
};

int main()
{
    std::string server_address = "0.0.0.0:4317";
    MetricsProxyService service;
    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();    
    return 0;    
}
