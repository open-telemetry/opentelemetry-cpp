include_directories(
  ${CMAKE_BINARY_DIR}/generated/third_party/opentelemetry-proto)
include_directories(${CMAKE_SOURCE_DIR}/exporters/otlp/include)

add_library(otlp_foo_library foo_library/foo_library.cc)
target_link_libraries(otlp_foo_library ${CMAKE_THREAD_LIBS_INIT}
                      ${CORE_RUNTIME_LIBS} opentelemetry_api)

add_executable(example_otlp main.cc)
target_link_libraries(
  example_otlp
  ${CMAKE_THREAD_LIBS_INIT}
  otlp_foo_library
  opentelemetry_trace
  ${CORE_RUNTIME_LIBS}
  opentelemetry_exporter_otprotocol
  gRPC::grpc++)
