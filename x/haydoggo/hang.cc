#include <iostream>
#include <thread>
#include <opentelemetry/exporters/ostream/span_exporter_factory.h>
#include <opentelemetry/sdk/trace/batch_span_processor_factory.h>
#include <opentelemetry/sdk/trace/tracer_provider_factory.h>

namespace trace = opentelemetry::sdk::trace;
/* thread_local */

std::unique_ptr<trace::TracerProvider> provider;

void workerThread() {
    auto exporter = opentelemetry::exporter::trace::OStreamSpanExporterFactory::Create(std::cout);
    trace::BatchSpanProcessorOptions processorOptions;
    auto processor = trace::BatchSpanProcessorFactory::Create(std::move(exporter), processorOptions);
    provider = trace::TracerProviderFactory::Create(std::move(processor));
    std::cout << "Provider created\n";
}

int main()
{
    auto thread = std::thread(workerThread);
    thread.join();
    std::cout << "Thread joined\n"; //Execution never reaches this point
}
