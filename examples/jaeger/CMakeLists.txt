include_directories(${CMAKE_SOURCE_DIR}/exporters/jaeger/include)

add_executable(example_jaeger main.cc)
target_link_libraries(
  example_jaeger ${CMAKE_THREAD_LIBS_INIT} common_foo_library
  opentelemetry_trace opentelemetry_exporter_jaeger_trace)
