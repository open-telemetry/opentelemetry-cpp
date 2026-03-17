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

## Auxiliary functions

This example provides a small set of auxiliary functions that are able to
create child spans from a previous one, independently on the active span:

- `CreateChildSpan`: Creating Child Spans for a given explicit parent.
- `CreateChildSpanFromRemote`: Creating Child Spans from incoming carriers
(i.e. headers).
- `InjectTraceContext`: Injecting arbitrary spans into carriers (i.e. headers).

> It's worth noting that `shared_ptr`'s are used because it helps for keeping
> spans alive and passing them across different lambda functions or execution
> scopes for more complex use cases.

## Example Flow

- This example creates 2 asynchronous requests from a theoretical client. The
requests have distributed tracing context `injected` into a map, simulating
headers.

- These requests then arrive to a server, which `extracts` the span information
and creates child spans sharing the same trace id's with the client request.
After that, other spans are `nested`, simulating server work.

- Answers contain again the context `injected`, and the client propagates it
without more context than the headers arriving in the answer.

- The parent spans that originated the only 2 `trace-id`'s simulated here are
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

## Output

The output will be a set of spans, in which you can check how the relationships
outlined in the previous diagrams are fulfilled. For example:

### Trace 1 `29656cc8079bff4fe30bdb96b0f24bef`

| name | parent_span_id | span_id | events |
| ------ | ---------------- | --------- | -------- |
| request1 | 0000000000000000 | f026e45ec526047e | _(none)_ |
| server | f026e45ec526047e | d701525271ff5d72 | Processing in server, Replying answer |
| nested | d701525271ff5d72 | c6aaee683b544ec3 | Nested did some work |
| process_answer | d701525271ff5d72 | fedcd1e2a22a1916 | Answer processed |

### Trace 2 `24a0afe30c007794d43dac03c4b7c956`

| name | parent_span_id | span_id | events |
| ------ | ---------------- | --------- | -------- |
| request2 | 0000000000000000 | a038787bb5eb6f1b | _(none)_ |
| server | a038787bb5eb6f1b | 529c7df6581d9279 | Processing in server, Replying answer |
| nested | 529c7df6581d9279 | d4946bb9738ae08b | Nested did some work |
| process_answer | 529c7df6581d9279 | 2b396d7d207addca | Answer processed |
