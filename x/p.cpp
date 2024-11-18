#include <memory>
#include <opentelemetry/exporters/otlp/otlp_grpc_forward_proxy.h>

int main()
{
    using namespace opentelemetry::exporter::otlp;
    
    OtlpGrpcMetricExporterOptions metricExporterOptions;
    metricExporterOptions.endpoint = "motel.cat.fact.com:4317";

    auto proxy{ std::make_unique<OtlpGrpcForwardProxy>() };

    proxy->AddListenAddress("0.0.0.0:4317");
    proxy->RegisterMetricExporter(metricExporterOptions);
    proxy->Start();
    proxy->Wait();
    return 0;    
}
