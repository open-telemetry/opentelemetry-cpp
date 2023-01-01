#include <opentracing/ext/tags.h>
#include <opentracing/noop.h>
#include <opentracing/tracer.h>
using namespace opentracing;

#define CATCH_CONFIG_MAIN
#include <opentracing/catch2/catch.hpp>

TEST_CASE("tracer") {
  auto tracer = MakeNoopTracer();

  auto span1 = tracer->StartSpan("a");
  CHECK(span1);

  SECTION("Spans provide references to the tracer that created them.") {
    CHECK(&span1->tracer() == tracer.get());
  }

  SECTION("Ensure basic operations compile.") {
    auto span2 = tracer->StartSpan("b", {ChildOf(&span1->context())});
    CHECK(span2);
    span2->SetOperationName("b1");
    span2->SetTag("x", true);
    span2->SetTag(opentracing::ext::span_kind,
                  opentracing::ext::span_kind_rpc_client);
    CHECK(span2->BaggageItem("y").empty());
    span2->Log({{"event", "xyz"}, {"abc", 123}});
    span2->Finish();
  }

  SECTION("A reference to a null SpanContext is ignored.") {
    StartSpanOptions options;
    ChildOf(nullptr).Apply(options);
    CHECK(options.references.size() == 0);
  }
}

TEST_CASE("A tracer can be globally registered") {
  CHECK(!Tracer::IsGlobalTracerRegistered());
  auto tracer = MakeNoopTracer();
  CHECK(Tracer::InitGlobal(tracer) != nullptr);
  CHECK(Tracer::IsGlobalTracerRegistered());
}
