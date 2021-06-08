include_directories(
  ${CMAKE_BINARY_DIR}/generated/third_party/opentelemetry-proto)
include_directories(${CMAKE_SOURCE_DIR}/exporters/otlp/include)

add_library(otlp_foo_library foo_library/foo_library.cc)
target_link_libraries(otlp_foo_library ${CMAKE_THREAD_LIBS_INIT}
                      ${CORE_RUNTIME_LIBS} opentelemetry_api)

if(WITH_OTLP_GRPC)
  add_executable(example_otlp_grpc grpc_main.cc)
  target_link_libraries(
    example_otlp_grpc
    ${CMAKE_THREAD_LIBS_INIT}
    otlp_foo_library
    opentelemetry_trace
    ${CORE_RUNTIME_LIBS}
    opentelemetry_exporter_otlp_grpc
    gRPC::grpc++)
endif()

if(WITH_OTLP_HTTP)
  add_executable(example_otlp_http http_main.cc)
  target_link_libraries(
    example_otlp_http ${CMAKE_THREAD_LIBS_INIT} otlp_foo_library
    opentelemetry_trace ${CORE_RUNTIME_LIBS} opentelemetry_exporter_otlp_http)
endif()
