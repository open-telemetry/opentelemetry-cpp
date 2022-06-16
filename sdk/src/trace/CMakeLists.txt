add_library(
  opentelemetry_trace
  tracer_context.cc
  tracer_provider.cc
  tracer.cc
  span.cc
  batch_span_processor.cc
  samplers/parent.cc
  samplers/trace_id_ratio.cc
  random_id_generator.cc)

set_target_properties(opentelemetry_trace PROPERTIES EXPORT_NAME trace)

target_link_libraries(opentelemetry_trace PUBLIC opentelemetry_common
                                                 opentelemetry_resources)

target_include_directories(
  opentelemetry_trace
  PUBLIC "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/sdk/include>")

install(
  TARGETS opentelemetry_trace
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
