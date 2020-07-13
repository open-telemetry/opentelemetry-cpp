# zPages
> Last updated 7/13/20

# Table of Contents
- [Summary](#summary)
  - [TraceZ](#tracez)
  - [RPCz](#rpcz)
- [Usage](#usage)
- [Links of Interest](#links-of-interest)

## Summary
zPages allow easy viewing of tracing information. When included for a process, zPages will display basic information about that process on a webpage. Two types of zPages include TraceZ and RPCz.

Including a zPage within a page is useful for developers because it's quicker to get running than adding extra code and installing external exporters like Jaeger and Zipkin. zPages tend to be more lightweight than these external exporters, but are also helpful for debugging latency issues (slow parts of applications) and deadlocks (running spans that don't end).

The idea of "zPages" originates from one of OpenTelemetry's predecessors, [OpenCensus](https://opencensus.io/). You can read more about it [here](https://opencensus.io/zpages). OpenCensus has different zPage implementations in [Java](https://opencensus.io/zpages/java/), [Go](https://opencensus.io/zpages/go/), and [Node](https://opencensus.io/zpages/node/) and there has been similar internal solutions developed at companies like Uber, but *this is the first major open-source implementation of zPages in C++*. Within OpenTelemetry, zPages are also being developed in [Java](https://github.com/open-telemetry/opentelemetry-java).

#### How It Works
On a high level, an application creates spans using a Tracer Provider/Tracer who passes them to a Span Processor, which exports spans to the appropriate Data Aggregator that a Http Server displays information for.

> TODO: Add picture examples for span overview and individual span view

### TraceZ
TraceZ is a type of zPage that shows information on tracing spans, and allows users to look closer at specific and individual spans. Details a user would view include span id, name, status, and timestamps. The individual components of TraceZ are as follows:

- TracezSpanProcessor (TSP)
  - A tracer/tracer provider (which the user chooses) creates spans, which connects to TSP so that TraceZ to detect spans. The TSP then stores tracing information in running and completed containers and provides an interface for TDA to access their information.
- TracezDataAggregator (TDA)
  - Intermediary between the TSP and THS, which also performs various functions and calculations (mainly grouping spans by their names and latency times) to send the correct tracing information to the THS.
- TracezHttpServer (THS)
  - User-facing web page generator, which creates HTML pages using TDA that display 1) overall information and trends on all of the process's spans and 2) more detailed information on specific spans when clicked.

### RPCz
RPCz is a type of zPage that provides details on instrumented sent and received RPC messages. Although there is currently no ongoing development of RPCz for OpenTelemetry, OpenCensus zPages have implementations of RPCz (linked above).

# Usage

> TODO: Add instructions to add zPages

## Links of Interest
- [TracezSpanProcessor Design Doc](https://docs.google.com/document/d/1kO4iZARYyr-EGBlY2VNM3ELU3iw6ZrC58Omup_YT-fU/edit#) (pending review)
- [TracezDataAggregator Design Doc](https://docs.google.com/document/d/1ziKFgvhXFfRXZjOlAHQRR-TzcNcTXzg1p2I9oPCEIoU/edit?ts=5ef0d177#heading=h.5irk4csrpu0y) (pending review)
- [TracezHttpServer Design Doc](https://docs.google.com/document/d/1U1V8QZ5LtGl4Mich-aJ6KZGLHrMIE8pWyspmzvnIefI/edit#) (draft)
- [zPages General Direction Spec](https://github.com/open-telemetry/oteps/blob/master/text/0110-z-pages.md)
- [Prospective Fields Displayed by
  TraceZ](https://github.com/open-telemetry/opentelemetry-cpp/blob/master/sdk/include/opentelemetry/sdk/trace/span_data.h)
