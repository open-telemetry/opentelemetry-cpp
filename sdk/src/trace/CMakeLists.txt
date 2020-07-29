add_library(
  opentelemetry_trace tracer_provider.cc tracer.cc span.cc
                      samplers/parent_or_else.cc samplers/probability.cc)
