include_directories(${CMAKE_SOURCE_DIR}/exporters/jaeger/include)

add_library(jaeger_foo_library foo_library/foo_library.cc)
target_link_libraries(jaeger_foo_library ${CMAKE_THREAD_LIBS_INIT}
                      ${CORE_RUNTIME_LIBS} opentelemetry_api)

add_executable(example_jaeger main.cc)
target_link_libraries(
  example_jaeger
  ${CMAKE_THREAD_LIBS_INIT}
  jaeger_foo_library
  opentelemetry_trace
  ${CORE_RUNTIME_LIBS}
  jaeger_trace_exporter)

