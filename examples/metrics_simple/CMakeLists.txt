include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include)

add_executable(simple_metrics main.cc)
target_link_libraries(
  simple_metrics ${CMAKE_THREAD_LIBS_INIT} opentelemetry_metrics_deprecated
  opentelemetry_exporter_ostream_metrics_deprecated)
