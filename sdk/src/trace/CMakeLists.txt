add_library(
  opentelemetry_trace
  tracer_provider.cc tracer.cc span.cc batch_span_processor.cc
  samplers/parent_or_else.cc samplers/probability.cc)

target_link_libraries(opentelemetry_trace opentelemetry_common ${CORE_RUNTIME_LIBS})
