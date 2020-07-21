# Built-in Samplers
The built-in samplers includes `AlwaysOn`, `AlwaysOff`, `Probability` and `ParentOrElse` samplers. This page explains relevant implementation details and how to use a sampler. For high-level design, please check out the [OpenTelemetry specification](https://github.com/open-telemetry/opentelemetry-specification/blob/master/specification/trace/sdk.md#built-in-samplers).


## TODOs
The `Probability` and `Parent-Or-Else` sampler depends on two significant functionalities that have not yet been implemented. Therefore, these two samplers require revisiting after implementing the following classes.
### SpanContext
Both `Probability` and `Parent-Or-Else` samplers require SpanContext of the parent span to make sampling decision. Since the SpanContext class hasn't been implemented yet, we currently use a mock class ([defined here](https://github.com/open-telemetry/opentelemetry-cpp/blob/master/api/include/opentelemetry/trace/span_context.h)) to store necessary information. To migrate from the mock SpanContext to the real class, please consider change the following:
#### sdk::trace::Tracer
In `StartSpan()`, current implementation passes in a `nullptr` to the sampler. Please consider changing the `nullptr` to a valid `SpanContext` pointer.
```
// Change the first parameter from nullptr to a pointer to SpanContext
auto sampling_result =
      sampler_->ShouldSample(nullptr, trace_api::TraceId(), name, options.kind, attributes);
```
#### sdk::trace::Sampler
Replace the mock SpanContext parameter in `ShouldSample()` with a real one. Apply the same change to all of the built-in sampler classes. 
```
// Change the first parameter parent_context
SamplingResult ShouldSample(const trace_api::SpanContext *parent_context,
                                      trace_api::TraceId trace_id,
                                      nostd::string_view name,
                                      trace_api::SpanKind span_kind,
                                      const trace_api::KeyValueIterable &attributes) noexcept = 0;
```

#### sdk::trace::ParentOrElseSampler
In `ShouldSample()` function, the sampler calls `parent_context->IsSampled()` to check the `SampledFlag` of its parent context. It might need to be updated if the `IsSampled()` changes.

#### sdk::trace::ProbabilitySampler

## How to use a sampler
### Constructing a sampler
Because we want to reuse a sampler whenever possible, it is a common practice to allocate a sampler dynamically using `std::make_share()`:
```
// Creating an AlwaysOn sampler.
std::shared_ptr<Sampler> always_on_sampler = std::make_shared<AlwaysOnSampler>();

// Creating an AlwaysOff sampler.
std::shared_ptr<Sampler> always_on_sampler = std::make_shared<AlwaysOffSampler>();

// Creating a parent-or-else sampler with an AlwaysOn delegate sampler.
std::shared_ptr<Sampler> parent_or_else_sampler = std::make_shared<ParentOrElseSampler>(always_on_sampler);

// Creating a tracer object with parent-or-else sampler
std::shared_ptr<opentelemetry::trace::Tracer> tracer = 
  std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor, sampler));

```
