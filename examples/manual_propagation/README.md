# OpenTelemetry C++ Manual Asynchronous Context Propagation Example

This example demonstrates how to manually propagate tracing context across
asynchronous calls between generic clients and servers. It simulates context
injections and extractions showing how to **explicitly/manually** set parent
span_id's in different situations, when the scope is not available, or
different threads are run in parallel.

The proposed pattern may be helpful for use cases which involve asynchronous
operations.

## Running the example

Build and Deploy this opentelemetry-cpp example as described in [INSTALL.md](../../INSTALL.md).

## Example Flow

* This example creates 2 asynchronous requests from a theoretical client. The
requests have distributed tracing context `injected` into a map, simulating
headers.

* These requests then arrive to a server, which `extracts` the span information
and creates child spans sharing the same trace id's with the client request.
After that, other spans are `nested`, simulating server work.

* Answers contain again the context `injected`, and the client propagates it
without more context than the headers arriving in the answer.

* The parent spans that originated the only 2 `trace-id`'s simulated here are
kept alive until the end of the example.

```text
[Client]                    [Server]

request1 span   ──────────→ server span
                                └─→ nested span
request2 span   ──────────→ server span
                                └─→ nested
                ←────────── reply
process_answer
                ←────────── reply
process_answer
```

## Span Hierarchy

Only 2 traces are generated in this example. Each one contains 4 spans in
total, and are propagated across a server and a client. The Span hierarchy is
shown below:

```text
request1  (trace: 29656cc8..., in client)
└─→ server (in server)
    ├─→ nested (in server)
    └─→ answer (in client)

request2  (trace: 24a0afe3...,  in client)
└─→ server
    ├─→ nested (in server)
    └─→ answer (in client)
```

## Auxiliary functions

This example provides a small set of auxiliary functions that are able to
create child spans from a previous one, independently on the active span.

It's worth noting that `shared_ptr`'s are used because it helps for keeping
spans alive and passing them across different lambda functions or execution
scopes for more complex use cases.

The auxiliary functions used in this example are explained below:

### Creating Child Spans

An auxiliary function that returns a child span for a desired arbitrary parent.
In this example, the `SpanKind` and `name` are also passed from outside the
function.

```cpp
nostd::shared_ptr<trace_api::Span> create_child_span(
    const std::string &name,
    const nostd::shared_ptr<trace_api::Span> &parent,
    trace_api::SpanKind kind)
{
  trace_api::StartSpanOptions opts;
  opts.kind = kind;
  if (parent)
  {
    opts.parent = parent->GetContext();
  }

  auto span =  get_tracer()->StartSpan(name, opts);
  return span;
}
```

### Creating Child Spans from incoming requests

An auxiliary function that returns a child span from incoming headers that
contain tracing information. In this example, the `SpanKind` and `name` are
also passed from outside the function.

```cpp
nostd::shared_ptr<trace_api::Span> create_child_span_from_remote(header_map &headers,
                                                                 const std::string &name,
                                                                trace_api::SpanKind kind)
{
  HttpTextMapCarrier carrier(headers);
  auto current_ctx = ctx::RuntimeContext::GetCurrent();
  auto new_context = ctx::propagation::GlobalTextMapPropagator::GetGlobalPropagator()->Extract(
      carrier, current_ctx);
  auto remote_span = opentelemetry::trace::GetSpan(new_context);

  return create_child_span(name, remote_span, kind);
}

```

### Injecting arbitrary spans into carriers (i.e. headers)

An auxiliary function that `injects` the tracing information to a given carrier
structure. In this example a simple implementation of the `TextMapCarrier` as
an `std::map<std::string, std::string>` was used.

It's important to note that the `trace_api::SetSpan(current_ctx, span)`
function is needed for the injection to explicitly use the desired span
context.

```cpp
void inject_trace_context(const nostd::shared_ptr<trace_api::Span> &span, header_map &headers)
{
  if (!span)
  {
    return;
  }

  // First, we set the Span into the context explicitly
  auto current_ctx   = ctx::RuntimeContext::GetCurrent();
  auto ctx_with_span = trace_api::SetSpan(current_ctx, span);

  // Then we inject the span info into the headers
  HttpTextMapCarrier carrier(headers);
  ctx::propagation::GlobalTextMapPropagator::GetGlobalPropagator()->Inject(carrier, ctx_with_span);
}
```

The previous examples make use of the following aliases:

```cpp
namespace trace_api = opentelemetry::trace;
namespace trace_sdk = opentelemetry::sdk::trace;
namespace nostd     = opentelemetry::nostd;
namespace ctx       = opentelemetry::context;
using header_map = std::map<std::string, std::string>;
```

## Output

The output will be a set of spans, in which you can check how the relationships
outlined in the previous diagrams are fulfilled. For example:

### Trace 1 `29656cc8079bff4fe30bdb96b0f24bef`

| name | parent_span_id | span_id | events |
|------|----------------|---------|--------|
| request1 | 0000000000000000 | f026e45ec526047e | _(none)_ |
| server | f026e45ec526047e | d701525271ff5d72 | Processing in server, Replying answer |
| nested | d701525271ff5d72 | c6aaee683b544ec3 | Nested did some work |
| process_answer | d701525271ff5d72 | fedcd1e2a22a1916 | Answer processed |

### Trace 2 `24a0afe30c007794d43dac03c4b7c956`

| name | parent_span_id | span_id | events |
|------|----------------|---------|--------|
| request2 | 0000000000000000 | a038787bb5eb6f1b | _(none)_ |
| server | a038787bb5eb6f1b | 529c7df6581d9279 | Processing in server, Replying answer |
| nested | 529c7df6581d9279 | d4946bb9738ae08b | Nested did some work |
| process_answer | 529c7df6581d9279 | 2b396d7d207addca | Answer processed |
