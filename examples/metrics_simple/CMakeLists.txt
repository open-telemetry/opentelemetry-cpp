include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include)
add_executable(metrics_ostream_example metrics_ostream.cc)
target_link_libraries(
  metrics_ostream_example ${CMAKE_THREAD_LIBS_INIT} opentelemetry_metrics
  opentelemetry_exporter_ostream_metrics opentelemetry_resources
  common_metrics_foo_library)
