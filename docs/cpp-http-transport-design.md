# HTTP transport requirements and options

## Why this document

The HTTP client under `ext/http/client/curl` has accumulated a set of reports
that no longer read as separate bugs. They cluster around a handful of places
where two parts of the code hold different models of the same thing, and each
individual fix has had to guess which model wins.

Discussion #4448 asks the question that has to come before any rewrite: does the
HTTP transport itself need to manage asynchronous concurrency, or is a simpler
request and result model enough, with concurrency owned somewhere else?

This document is written to answer that, not to propose class names. It sets out
what the transport is used for today, compares three shapes it could take, and
lists the decisions that have to be made before any of them can be built. It
does not assume that `HttpOperation` has to go, that a `Session` carries one
request, or where retry belongs.

The evidence behind it is the invariant matrix in #4448, which maps each report
to the invariant it broke and says which are measured and which are only
reasoned. That is deliberately not repeated here.

## What the transport is used for today

Every in-tree consumer, read from `main`:

| Consumer | Client | What it does |
| --- | --- | --- |
| Zipkin | `HttpClientSync` | Blocking by construction |
| Elasticsearch, default | async curl client | `handler->waitForResponse()` before `Export` returns |
| OTLP HTTP, default | async curl client | `Export(..., 0)` captures the result and waits |
| OTLP HTTP, async preview | async curl client | non-zero budget, returns with sessions outstanding |
| Elasticsearch, async preview | async curl client | returns with sessions outstanding |
| A user supplied client | anything | callbacks may run inline or on another thread |

`WITH_ASYNC_EXPORT_PREVIEW` is `OFF` by default.

So in the configuration almost everyone builds, every exporter blocks until its
request is done, and two of the three do it by handing the request to an
asynchronous client and then waiting for it. Those callers pay for a background
thread, a callback contract and a `CURLM` whether or not anything they can
observe depends on it.

That is one of the two facts this document turns on. The second one points the
other way and is just as load bearing.

## What the concurrency is for

An earlier version of this document read as an argument that the default
configuration should not carry the asynchronous machinery at all. @owent
supplied the counterweight on #4448, from production rather than from reasoning,
and it is worth stating in full because it changes the shape of the answer.

A high volume logging workload was dropping data. The collector was not the
bottleneck. Network latency was around 100 ms, the batch processor had already
merged each submission past 4 MB, and production simply outran what one request
in flight could consume. The drops stopped once roughly four requests were
allowed in flight at once.

The specification describes the same bound arithmetically. Maximum achievable
throughput is `max_concurrent_requests * max_request_size / (network_latency +
server_response_time)`, and it notes that in high latency networks the requests
have to be very big or a lot of concurrent requests have to be done. That is the
workload above, with the request size already at the top of its range.

The specification's language differs by transport, which matters here because
this document is about the HTTP one. For gRPC it says an implementation that
needs high throughput SHOULD support concurrent unary calls and that the number
SHOULD be configurable. For HTTP it says a client MAY send requests over several
parallel connections, and that the maximum number SHOULD be configurable.

Taken together the two facts do not select a side. They ask for a transport
where concurrency is available, bounded and configurable, and where a caller
that does not want it does not pay for it. Nothing below should be read as
proposing to remove the capability.

One number needs to be said out loud rather than assumed. The exporter's default
today is `max_concurrent_requests` 64 and `max_requests_per_connection` 8. The
workload above needed about four. Neither figure was derived from a benchmark in
this repository, and the gap between them is a question for measurement, not for
argument.

## Where the current model breaks

Five things are modelled inconsistently, and most reports are a consequence of
one of them.

**Operation identity.** A `Session` holds one operation that can be replaced.
The background thread recovers a `Session` from `CURLOPT_PRIVATE` and then asks
it for its current operation, which is not necessarily the operation the
completion belongs to.

**Ownership across an asynchronous boundary.** The operation borrows the request
headers, body and SSL options, the event handler as a raw pointer, and the
session as a raw pointer. Whether those outlive the transfer is left to
convention.

**Publication.** The operation, its promise, its callback, its cancel route and
its entry in the client's map become visible one at a time. User code can run
and another thread can cancel while that is still happening.

**Completion.** Progress and outcome are delivered through the same two
callbacks, and nothing states which states are terminal, whether exactly one
arrives, or who wins when a response and a cancel race.

**Backend resources.** The easy handle, the header list, multi membership, the
retry queue and the session registry are owned across three classes and two
threads.

Adding another mutex or atomic can only synchronise a model. It cannot decide
what the model is, which is why the individual fixes keep needing a contract
that does not exist yet.

## Option A: keep `Session`, `EventHandler` and `HttpOperation`

Repair the current shapes in place: publish atomically, define which states are
terminal, give the operation an identity, and settle resource ownership.

This is the smallest migration. Nothing installed changes, no consumer moves,
and the existing tests keep their meaning. It also keeps the asynchronous
capability that #1175 and #1243 were written for.

What it does not do is remove the cost from the default configuration. A caller
that blocks still pays for a background thread, a callback contract and a
`CURLM`. It also leaves the abstraction shaped by one backend: `EventHandler`
delivers curl's progress states, and a WinHTTP or `NSURLSession` implementation
would have to invent or ignore them.

## Option B: an implementation neutral one attempt operation

One attempt in, one result out, submitted asynchronously. The transport is
handed an immutable request and a deadline and returns a handle for that
submission. Exactly one of a response, an invalid request, a transport error, a
deadline or a cancellation eventually settles it.

The earlier draft of this section described the same contract as a blocking
call. That was the mistake @owent's evidence exposes: a blocking interface
cannot serve the workload above without a second model beside it, and two models
is how the current code got here. Asynchronous submission is the general shape,
and blocking is the special case of submitting one operation and waiting for
that operation's own result until its own deadline. Not `ForceFlush`, which
waits for everyone else's work as well, and which has its own open defects.

What this buys is that the properties that keep breaking become structural
rather than conventional. Exactly one outcome is what the handle settles to.
Immutability removes the borrowed request lifetime. The submission has an
identity that does not move when a session is reused. Progress reporting is
separable from settlement, so there is no progress callback that can be mistaken
for one.

The cost is real and should not be glossed. It is a second interface next to the
installed one, so it needs an adapter and a migration story rather than a
rename, and the concurrency limit, the retry policy and the queue have to live
somewhere. This document's position is that they belong above the transport and
below the exporter, but that is the decision to argue about rather than assume.

## Option C: a single owner `CURLM` event loop

If transport level concurrency is wanted, this is the shape libcurl's own rules
point at. One thread owns the multi handle and every easy handle attached to it.
Other threads only enqueue submit, cancel and shutdown commands and wake it with
`curl_multi_wakeup`. Every add, perform, remove and cleanup happens on the owner
thread, and user completions run after leaving libcurl's callback stack and
without holding any client lock.

libcurl documents that one handle must not be used by two threads at once, that
a transfer's easy handle stays attached until it is explicitly removed, and that
removal has to come before cleanup. A single owner satisfies all three by
construction rather than by review.

This is a backend implementation choice. It does not have to appear in the
public interface, and it should not: a native Windows or Apple transport has no
multi handle to own.

## Against the criteria in #4448

| | A: current model | B: one attempt operation | C: single owner loop |
| --- | --- | --- | --- |
| Throughput, connection reuse | keeps both | reuse yes, in-flight count owned above it | keeps both |
| Cancellation | flag plus races | explicit, at a defined point | explicit, owner thread applies it |
| Shutdown | four reports open | bounded by construction | bounded, one thread to drain |
| Retry and concurrency owner | inside the transport | left open, above the transport | inside the backend |
| Installed interfaces | unchanged | new interface plus an adapter | unchanged, backend only |
| Custom clients | must be re-entrant, undocumented | one method to implement | unaffected |
| Native backends | awkward | natural | not applicable |

## What this needs decided

These are the questions a design cannot avoid. They are listed as decisions
rather than answered here.

- [ ] Is a `Session` single use, reusable after completion, or a legacy adapter?
- [ ] Does submitting a request snapshot it or take ownership of it?
- [ ] Is completion one terminal result, with progress separated out as optional
      diagnostics?
- [ ] On what thread do callbacks run, and which calls may a callback make?
- [ ] Which layer owns retry, in-flight concurrency and `Retry-After`?
- [ ] What should the default number of requests in flight be, given that the
      option says 64 today and the one reported workload needed about four?
- [ ] Is a new contract introduced alongside the current one with an adapter, or
      does the current interface evolve?

The last one is not only an API question. CMake installs the abstract
interfaces and `http_client_factory_curl.h` but not the concrete curl headers,
while Bazel's `//ext:headers` still globs everything under `ext/include`. The
two surfaces disagree today, and whichever direction is chosen has to say what
happens to a Bazel consumer that includes a concrete header.

## A recommendation

This part is an opinion and is meant to be argued with.

I would define the one attempt semantics of option B as the contract, keep the
current interface working through an adapter rather than replacing it, and treat
option C as the curl backend's implementation for the case where concurrency is
actually wanted. Blocking and asynchronous callers then share one set of rules
about ownership, deadlines and settlement, and differ only in how the result is
presented.

Concretely that is three layers rather than one interface.

A portable one attempt operation over an immutable request, with an identity, a
deadline, idempotent cancellation and exactly one settlement. A bounded
scheduler above it owning how many requests are in flight, admission and
backpressure, retry and `Retry-After`, and what `ForceFlush` and `Shutdown`
mean. A backend below it, where the curl implementation can be a single owner
`CURLM` loop and WinHTTP or `NSURLSession` can use their own execution models
without either being visible through the contract. Blocking is then an adapter
over the first layer and not a second transport.

Putting the scheduler above the transport rather than inside it is the load
bearing choice, and the specification points the same way: concurrent requests
and retry are described as the exporter's responsibility. It is also the part I
am least certain of, because a native backend that already has its own queue
would then have two.

## What has to be measured before any of this is built

The concurrency question should be answered with a number, and the repository
does not currently have one. The only OTLP HTTP benchmark here runs against
localhost with a 1 ms timeout and no retry, which is the case where latency does
not exist.

The workload that has to be reproducible is the one from #4448, because it is
the only evidence anyone has offered for what concurrency is worth:

| Dimension | Values |
| --- | --- |
| Round trip time | 0, 1, 10, 50, 100, 250 ms |
| Server processing | 0, 10, 100 ms |
| Payload | 4 KiB, 64 KiB, 1 MiB, 4 MiB, 16 MiB |
| Requests in flight | 1, 2, 4, 8, 16, 64 |
| Connection policy | close, keep alive, HTTP/2 |
| Encoding | protobuf, JSON, with and without gzip |

Reported per run: acknowledged batches per second, dropped batches, p50 and p99
latency, resident memory, thread count, open descriptors, and whether
`ForceFlush` and `Shutdown` returned the right answer within their deadlines.

The `100 ms` in the report is not qualified as one way or round trip, so the
benchmark should define it as `netem` round trip and say so rather than inherit
the ambiguity. Until that exists, "about four" is a credible first hand
observation and not a number this project can defend or regress against.

## On the C++ baseline

The current baseline is C++14, and nothing above depends on changing it.

C++17 would remove some of the awkwardness rather than change the design.
`std::optional` and `std::variant` express a result that is exactly one of
several things without a sentinel, and `std::string_view` removes some copies at
the request boundary. C++20 coroutines would make an asynchronous adapter easier
to write but would also make it the natural way to consume the transport, which
is a larger commitment than this document is asking for.

The recommendation is to design for C++14 and take the simplifications if and
when the baseline moves, rather than to wait for it.
