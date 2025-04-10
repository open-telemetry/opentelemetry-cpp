#include <iostream>

#ifdef _WIN32
#define IMPORT __declspec(dllimport)
#else
#define IMPORT
#endif

#include <opentelemetry/exporters/ostream/span_exporter_factory.h>
#include <opentelemetry/sdk/trace/processor.h>
#include <opentelemetry/sdk/trace/simple_processor_factory.h>
#include <opentelemetry/sdk/trace/tracer_provider_factory.h>
#include <opentelemetry/trace/provider.h>


using namespace std;
namespace trace      = opentelemetry::trace;
namespace exporter   = opentelemetry::exporter;
namespace trace_sdk  = opentelemetry::sdk::trace;
namespace nostd      = opentelemetry::nostd;


extern "C" IMPORT void foo();


int main(int argc, const char* argv[])
try
{
  // Configure trace provider that exports to stdout

  using prv_factory = trace_sdk::TracerProviderFactory;
  using prc_factory = trace_sdk::SimpleSpanProcessorFactory;
  using exp_factory = exporter::trace::OStreamSpanExporterFactory;

  trace::Provider::SetTracerProvider(
    std::shared_ptr<trace::TracerProvider>{
      prv_factory::Create(prc_factory::Create(exp_factory::Create(std::cout)))
    }
  );

  // Get provider and create a span

  auto provider = trace::Provider::GetTracerProvider();
  auto tracer = provider->GetTracer("try", "0.0.1");
  auto span = tracer->StartSpan("main");
  trace::Scope scope{tracer->WithActiveSpan(span)};

  // Call library function that creates another span.
  // Expecting that it will use trace provider configured above.

  foo();

  cout << "Done!" << endl;
}
catch(const char *e)
{
  cout << "ERROR: " << e << endl;
  exit(1);
}
catch(std::exception &e)
{
  cout << "EXCEPTION: " << e.what() << endl;
  exit(1);
}