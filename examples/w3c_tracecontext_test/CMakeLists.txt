include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include)

find_package(CURL)
if(CURL_FOUND)
  add_executable(example_w3c_tracecontext_test main.cc)
  target_link_libraries(
    example_w3c_tracecontext_test ${CMAKE_THREAD_LIBS_INIT} opentelemetry_trace
    opentelemetry_exporter_ostream_span ${CURL_LIBRARIES})
endif()
