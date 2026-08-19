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
asynchronous client and then waiting for it. The background thread, the callback
machinery and the `CURLM` bookkeeping are all paid for and none of it is used
for anything the caller can observe.

That is the fact the rest of this document turns on.

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

## Option B: an implementation neutral request and result

One attempt in, one result out. The transport is handed an immutable request and
a deadline, and produces exactly one of a response, an invalid request, a
transport error, a deadline, or a cancellation.

This matches what every default consumer already does, and it makes the
properties that keep breaking structural rather than conventional. Exactly one
outcome is what the interface returns. Immutability removes the borrowed request
lifetime. There is no progress callback to re-enter from.

The cost is that it does not by itself serve the async preview or a native
asynchronous backend. Something has to sit between a blocking interface and a
platform that only offers completion callbacks, and where that adapter lives is
a real design question rather than a detail.

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

| | A: current model | B: request and result | C: single owner loop |
| --- | --- | --- | --- |
| Throughput, connection reuse | keeps both | reuse yes, parallelism needs a layer above | keeps both |
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

I would also want the concurrency question answered with a measurement rather
than an assumption. The OTLP specification suggests sequential requests are
reasonable against a local collector and that concurrency matters as round trip
time grows. If sequential requests with connection reuse are enough for a local
collector, the default configuration should not carry the machinery, and the
async preview becomes the thing that opts into it.

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
