#include "opentelemetry/ext/zpages/tracez_processor.h"

#include <benchmark/benchmark.h>

#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/nostd/shared_ptr.h"

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::ext::zpages;

//////////////////////////////////// TEST HELPER FUNCTIONS //////////////////////////////

/*
 * Helper function that creates i spans, added into the passed in vector
 */
void StartManySameSpans(
    std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> &spans,
    std::shared_ptr<opentelemetry::trace::Tracer> tracer,
    int i)
{
  for (; i > 0; i--)
    spans.push_back(tracer->StartSpan("span"));
}

/*
 * Helper function that ends all spans in the passed in span vector.
 */
void EndSpans(std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> &spans)
{
  for (auto &span : spans)
    span->End();
}

//////////////////////////////// TEST FIXTURE //////////////////////////////////////

/*
 * Reduce code duplication by having single area with shared setup code
 */
class TracezProcessorBM : public benchmark::Fixture
{
protected:
  void SetUp(const ::benchmark::State& state)
  {
    processor  = std::shared_ptr<TracezSpanProcessor>(new TracezSpanProcessor());
    tracer     = std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));

  }

  std::shared_ptr<TracezSpanProcessor> processor;
  std::shared_ptr<opentelemetry::trace::Tracer> tracer;

  std::vector<opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>> spans;

  const int numIterations = 1000;
  const int numSpans      = 500;
};

///////////////////////////////////////// TESTS ///////////////////////////////////

BENCHMARK_F(TracezProcessorBM, BM_TracezProcessorManySameName)(benchmark::State &state)
{
  while(state.KeepRunningBatch(numIterations))
  {
    StartManySameSpans(spans, tracer, 500);
    EndSpans(spans);
  }
}


BENCHMARK_MAIN();
