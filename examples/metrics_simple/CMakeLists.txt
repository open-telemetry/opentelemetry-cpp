include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include)
if(WITH_METRICS_PREVIEW)
  add_executable(simple_metrics main.cc)
  target_link_libraries(
    simple_metrics ${CMAKE_THREAD_LIBS_INIT} opentelemetry_metrics_deprecated
    opentelemetry_exporter_ostream_metrics_deprecated)
else()
  add_executable(metrics_ostream_example metrics_ostream.cc)
  target_link_libraries(
    metrics_ostream_example ${CMAKE_THREAD_LIBS_INIT} opentelemetry_metrics
    opentelemetry_exporter_ostream_metrics opentelemetry_resources
    common_metrics_foo_library)
endif()
