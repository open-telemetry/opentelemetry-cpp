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
where concurrency is available, bounded and configurable. What a blocking caller
should not need is a second set of semantics; whether it also avoids a
background thread is a cost to measure rather than something to promise here.
Nothing below should be read as proposing to remove the capability.

One number needs to be said out loud rather than assumed. The exporter's default
today is `max_concurrent_requests` 64 and `max_requests_per_connection` 8. The
workload above needed about four. Neither figure was derived from a benchmark in
this repository, and the gap between them is a question for measurement, not for
argument.

## What the two existing options actually do

@owent's second reply on #4448 says these were written around curl and that a
replacement need not be bound by them. Reading them settles what a replacement
would have to keep, which is less than their names suggest.

`max_concurrent_requests`, default 64, is not an admission limit. `Export()`
builds the request, hands it to the HTTP client, and only then waits for the
count of running sessions to fall below it. It is a watermark applied after
submission, so the request already exists in memory by the time it applies.

`max_requests_per_connection`, default 8, is not a per connection counter.
Nothing counts requests against a connection. `Session::SendRequest` computes

```cpp
reuse_connection = session_id_ % max_sessions_per_connection != 0;
```

and every Nth session by global id then sets `CURLOPT_FRESH_CONNECT` and
`CURLOPT_FORBID_REUSE`, which opens a new connection and closes it after one
use. It is a periodic rotation keyed on a counter that spans every session, not
a quota.

Neither controls the shape of what is on the wire, because the client never asks
for it. Grepping `ext/` and `exporters/` for the options that would:

| option | set anywhere |
| --- | --- |
| `CURLMOPT_MAX_HOST_CONNECTIONS` | no |
| `CURLMOPT_MAX_TOTAL_CONNECTIONS` | no |
| `CURLMOPT_MAX_CONCURRENT_STREAMS` | no |
| `CURLMOPT_PIPELINING` | no |
| `CURLOPT_HTTP_VERSION` | no |

So the connection count, the stream count and the protocol are all whatever
libcurl negotiates, and the same two option values can mean different things
against different servers or against a different system libcurl. That is worth
fixing whatever contract wins.

## Where "queued and executed serially" holds

@owent's reply says that adding requests to one `CURLM` without disabling reuse
can look concurrent while running serially underneath. That is right, and the
qualifier matters for what the replacement has to expose.

It holds on one connection that cannot multiplex. HTTP/1.1 responses arrive in
request order, and libcurl removed HTTP/1 pipelining, so in practice one such
connection carries one exchange at a time. It does not hold generally: a `CURLM`
may open several connections to the same host, and HTTP/2 and HTTP/3 carry
several streams on one connection subject to the server's own limits.

Which is why one number cannot express it. A replacement has to separate

- export operations admitted and in flight,
- HTTP attempts, since one operation may make several,
- physical connections,
- streams per connection,
- and bytes retained for requests that have not settled.

`max_concurrent_requests` today stands in for the first and, through the
rotation heuristic, indirectly for the third. It says nothing about the rest.

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

## The words this needs, since one of them is doing too much work

`max_concurrent_requests` currently stands for several different quantities, and
the sections above only make sense if they are named apart.

| term | what it counts |
| --- | --- |
| record rate | records the application produces per second |
| export operation | one batch, from first attempt to final outcome |
| attempt | one HTTP request; a retried operation has several |
| admitted | attempts the governor has let start |
| pending | attempts waiting for capacity rather than for the network |
| running transfer | attempts libcurl is currently working on |
| connection | one TCP or QUIC connection to an origin |
| stream | one exchange on a connection; one at a time on HTTP/1.1 |
| retained bytes | serialized request bytes held for attempts that have not settled |

The reported workload is a record rate. The "about four" is an attempt count.
They are not the same number and a benchmark that reports one as the other
cannot be compared with anything.

## Against the criteria in #4448

| | A: current model | B: one attempt operation | C: single owner loop |
| --- | --- | --- | --- |
| Throughput, connection reuse | keeps both | reuse yes, in-flight count owned above it | keeps both |
| Cancellation | flag plus races | explicit, at a defined point | explicit, owner thread applies it |
| Shutdown | four reports open | bounded by construction | bounded, one thread to drain |
| Admission and retry owner | inside the transport | above the transport | above the transport |
| Connection and stream scheduling | implicit, libcurl decides | backend policy | backend policy, one owner thread |
| Installed interfaces | unchanged | new interface plus an adapter | unchanged, backend only |
| Custom clients | must be re-entrant, undocumented | one method to implement | unaffected |
| Native backends | awkward | natural | not applicable |

## What this needs decided

Two lists rather than one. The first is what the discussion on #4448 has already
settled, written down so a reviewer can disagree with a specific line instead of
re-reading the thread. The second is what is genuinely still open.

**Settled, unless someone objects to a line here.** A backend neutral contract
rather than a curl shaped one. Asynchronous submission of one attempt over a
request that does not change after it is submitted. Exactly one settlement per
export operation. Bounded resources, in bytes as well as in count. The existing
interface kept working through an adapter rather than replaced. One owner thread
per `CURLM`. The two existing options treated as curl compatibility controls
rather than as requirements of the replacement.

**Open.** These are the questions a design cannot avoid, and they are listed
rather than answered.

- [ ] Is a `Session` single use, reusable after completion, or a legacy adapter?
- [ ] Does submitting a request snapshot it or take ownership of it?
- [ ] Is completion one terminal result, with progress separated out as optional
      diagnostics?
- [ ] On what thread do callbacks run, and which calls may a callback make?
- [ ] Which layer owns retry, in-flight concurrency and `Retry-After`?
- [ ] What should the default number of requests in flight be, given that the
      option says 64 today and the one reported workload needed about four?
- [ ] Is the budget a count of requests, a number of bytes retained, or both?
      64 requests at the 4 MB the reported workload was sending is 256 MB of
      request payload before responses, retries or compression buffers.
- [ ] Do an export operation and an HTTP attempt get separate identities, so
      that a retry is one operation and several attempts?
- [ ] Is a new contract introduced alongside the current one with an adapter, or
      does the current interface evolve?

The last one is not only an API question. CMake installs the abstract
interfaces and `http_client_factory_curl.h` but not the concrete curl headers,
while Bazel's `//ext:headers` still globs everything under `ext/include`. The
two surfaces disagree today, and whichever direction is chosen has to say what
happens to a Bazel consumer that includes a concrete header.

## What a replacement has to hold, whichever shape wins

These are requirements rather than questions, because no answer to the open
decisions below makes any of them optional.

- Total bytes retained for attempts that have not settled is bounded. A count of
  requests does not bound it: 64 in flight at the 4 MB the reported workload was
  sending is 256 MB of request payload.
- Compression working memory counts against that bound. Today the gzip step
  works in the caller's own buffer, so it is invisible to any request count.
- A retry does not silently multiply retained payload. One export operation
  holds one serialized body however many attempts it makes.
- An export operation and an attempt have separate identities. `ForceFlush`
  answers for operations accepted before its watermark, not for whichever
  attempts happen to be running.
- Exactly one outcome is delivered per export operation, and it is the
  operation's, not the last attempt's.

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

Rates have to name their unit. The reported workload is "roughly 10 to 20K QPS"
of log volume, which is not the same number as HTTP requests per second, so the
run should report both, along with records and serialized bytes per request.
Otherwise the figure cannot be compared with anything.

The run also has to read back what libcurl actually did, not only what it was
asked for: `CURLINFO_HTTP_VERSION` for the negotiated protocol and
`CURLINFO_NUM_CONNECTS` for connections opened. Without those, a result at four
in flight cannot be told apart from four connections, four streams on one
connection, or four requests serialized behind each other.

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
