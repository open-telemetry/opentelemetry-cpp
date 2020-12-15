add_library(
  opentelemetry_trace
  tracer_provider.cc tracer.cc span.cc batch_span_processor.cc
  fork_aware_batch_span_processor.cc fork_aware_span_processor.cc
  fork_aware_exporter.cc samplers/parent.cc samplers/trace_id_ratio.cc)

target_link_libraries(opentelemetry_trace opentelemetry_common)
