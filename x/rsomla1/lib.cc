#include <iostream>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif


#include <opentelemetry/trace/provider.h>

namespace trace      = opentelemetry::trace;
using namespace std;

extern "C" EXPORT void foo()
{
  auto provider = trace::Provider::GetTracerProvider();
  auto tracer = provider->GetTracer("lib", "0.0.1");
  auto span = tracer->StartSpan("foo");

  cout << "foo DONE!" << endl;
}