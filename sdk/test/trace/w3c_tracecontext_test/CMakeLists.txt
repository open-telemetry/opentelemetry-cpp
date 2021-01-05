include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include)

find_package(CURL)

if(CURL_FOUND)
  add_executable(w3c_tracecontext_test main.cc)
  target_link_libraries(
    w3c_tracecontext_test ${CMAKE_THREAD_LIBS_INIT} opentelemetry_trace
    opentelemetry_exporter_ostream_span ${CURL_LIBRARIES})
else()
  message(WARNING "Skipping example_w3c_tracecontext_test: CURL not found")
endif()
