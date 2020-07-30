# Built-in sampler example

The built-in samplers includes `AlwaysOn` , `AlwaysOff` , `Probability` and `ParentOrElse` samplers. For high-level design, please check out the [OpenTelemetry specification](https://github.com/open-telemetry/opentelemetry-specification/blob/master/specification/trace/sdk.md#built-in-samplers). In this page, you will go through an example to learn how to use a sampler in the `TracerProvider` .

## Getting started

1. Make sure you are at `examples\simple-sampler`
2. Run `bazel build :example_simple` to build the program.
3. The compiled binary will be in the `bazel-bin\` folder under rood directory. 
4. Run `../../bazel-bin/examples/simple-sampler/example_simple` . It will test AlwaysOn and AlwaysOff sampler.

## Understanding the example

### Including the sampler header file

The samplers are under `sdk` folder. You can include them by adding:

``` 
#include "opentelemetry/sdk/trace/sampler.h"
#include "opentelemetry/sdk/trace/samplers/always_off.h"
#include "opentelemetry/sdk/trace/samplers/always_on.h"
#include "opentelemetry/sdk/trace/samplers/parent_or_else.h"
```

### Constructing a sampler

It is a common practice to use `std::make_shared` to allocate a sampler dynamically so that it can be used by multiple `Tracer` s. In the `main()` function, we first construct a few samplers. 

``` 
auto always_on_sampler  = std::make_shared<AlwaysOnSampler>();
auto always_off_sampler = std::make_shared<AlwaysOffSampler>();
// A Parent-Or-Else sampler with a delegate AlwaysOn sampler.
// This sampler requires SpanContext, so is not yet fully functional.
auto parent_or_else_sampler = std::make_shared<ParentOrElseSampler>(always_on_sampler);
```

### Constructing a Tracer Provider with a sampler

You can pass in a `std::shared_ptr<Sampler>` to the TracerProvider constuctor. In the sample program, you first create a `SimpleSpanProcessor` using `StdoutExporter` , and then construct the `TracerProvider` with the processor and the sampler. In the example, we put these steps into `void initTracer(std::shared_ptr<Sampler> sampler)` .

``` 
void initTracer(std::shared_ptr<Sampler> sampler)
{
  auto exporter  = std::unique_ptr<sdktrace::SpanExporter>(new StdoutExporter);
  auto processor = std::shared_ptr<sdktrace::SpanProcessor>(
      new sdktrace::SimpleSpanProcessor(std::move(exporter)));
  auto provider =
      nostd::shared_ptr<trace::TracerProvider>(new sdktrace::TracerProvider(processor, sampler));
  // Set the global trace provider
  trace::Provider::SetTracerProvider(provider);
}
```

### Expected output

When the `TracerProvider` is using the AlwaysOn sampler, it will record the information from every span, and export it to `stdout` . When it is using AlwaysOff sampler, it will record nothing.  
