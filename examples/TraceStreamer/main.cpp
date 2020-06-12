//
// Example that illustrates the following concepts:
// - how to use ILogger-style event API
// - how to attach event::Properties object to span
// - how to implement a custom Tracer
//

#include <opentelemetry/trace/key_value_iterable_view.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/span_id.h>
#include <opentelemetry/trace/trace_id.h>
#include <opentelemetry/trace/tracer_provider.h>

#include <opentelemetry/nostd/stltypes.h>

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <map>
#include <string_view>

#include "StreamTracer.hpp"

using namespace OPENTELEMETRY_NAMESPACE;

namespace trace = opentelemetry::trace;

// using M = std::map<std::string_view, std::string_view>;

using M = std::map<nostd::string_view, nostd::string_view>;

#ifdef __has_include
#if __has_include(<benchmark/benchmark.h>)
#include <benchmark/benchmark.h>
#define HAVE_BENCHMARK
#endif
#endif

#ifdef HAVE_BENCHMARK
std::map<nostd::string_view, common::AttributeValue> createEvent(){
  std::map<nostd::string_view, common::AttributeValue> m =
  {
      {"uint32Key", (uint32_t)123456},
      {"uint64Key", (uint64_t)123456},
      {"strKey", "someValue"}
  };
  return m;
};

void BM_AddEvent(benchmark::State &state)
{
  stream::TracerProvider tp;
  auto tracer = tp.GetTracer("json", "NUL");
  auto span   = tracer->StartSpan("MySpan");
  while (state.KeepRunning())
  {
    benchmark::DoNotOptimize([&]
        {
            span->AddEvent("MyEvent", createEvent());
        });
  }
  span->End();
  tracer->Close();
}
BENCHMARK(BM_AddEvent);

#endif

std::multimap<std::string, std::string> testParams = {
    // Plain text key-value pairs logfile
    {"file", "trace.log"},
    // JSON text file
    {"json", "trace.json"},
#ifdef _WIN32
    // JSON pipe listener (Windows)
    {"json", "\\\\.\\pipe\\ETW-6d084bbf-6a96-44ef-83F4-0a77c9e34580"},
    // ETW-XML listener (Windows)
    {"ETW", "{6D084BBF-6A96-44EF-83F4-0A77C9E34580}"},
#endif
    // Console
    {"CON", "1.0"},
    // OutputDebugString (Visual Studio Debug Output window)
    {"DEBUG", "1.0"}
};

void test_spans()
{
  for (auto &kv : testParams)
  {
    printf("*** Tracer(%s:%s)...\n", kv.first.c_str(), kv.second.c_str());
    stream::TracerProvider tp;
    auto tracer = tp.GetTracer(kv.first, kv.second);
    auto span   = tracer->StartSpan("MySpan");

    // add m1 to span 1
    M m1 = {{"key1", "one"}, {"key2", "two"}};
    span->AddEvent("MyProduct.MyEvent1", m1);

    // add m2 to span 2
    M m2 = {{"key1", "one"}, {"key2", "two"}};
    span->AddEvent("MyProduct.MyEvent2", m2);

    // add map to span using initializer_list
    span->AddEvent("MyProduct.MyEvent3", {{"key1", "one"}, {"key2", "two"}});

    span->End();
    // end tracing session
    tracer->Close();
  }
}

int main(int argc, char *argv[])
{
  test_spans();

#ifdef HAVE_BENCHMARK
  // Run the benchmark
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
#endif

  return 0;
}
