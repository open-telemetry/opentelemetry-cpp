include_directories(${CMAKE_SOURCE_DIR}/exporters/zipkin/include)

add_library(zipkin_foo_library foo_library/foo_library.cc)
target_link_libraries(zipkin_foo_library ${CMAKE_THREAD_LIBS_INIT}
                      opentelemetry_api)

add_executable(example_zipkin main.cc)
target_link_libraries(
  example_zipkin ${CMAKE_THREAD_LIBS_INIT} zipkin_foo_library
  opentelemetry_trace opentelemetry_exporter_zipkin_trace)
