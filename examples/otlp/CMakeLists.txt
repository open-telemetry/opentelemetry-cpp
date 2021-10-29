include_directories(
  ${CMAKE_BINARY_DIR}/generated/third_party/opentelemetry-proto
  ${CMAKE_SOURCE_DIR}/examples/common
  ${CMAKE_SOURCE_DIR}/exporters/otlp/include)

if(WITH_OTLP_GRPC)
  add_executable(example_otlp_grpc grpc_main.cc)
  target_link_libraries(
    example_otlp_grpc ${CMAKE_THREAD_LIBS_INIT} common_foo_library
    opentelemetry_trace opentelemetry_exporter_otlp_grpc gRPC::grpc++)
endif()

if(WITH_OTLP_HTTP)
  add_executable(example_otlp_http http_main.cc)
  target_link_libraries(
    example_otlp_http ${CMAKE_THREAD_LIBS_INIT} common_foo_library
    opentelemetry_trace opentelemetry_exporter_otlp_http)
endif()
