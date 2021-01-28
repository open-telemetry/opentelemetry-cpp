include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include)

find_package(CURL)
find_package(nlohmann_json)

if(NOT CURL_FOUND)
  message(WARNING "Skipping example_w3c_tracecontext_test: CURL not found")
elseif(NOT nlohmann_json_FOUND)
  message(
    WARNING "Skipping example_w3c_tracecontext_test: nlohmann_json not found")
else()
  add_executable(w3c_tracecontext_test main.cc)
  target_link_libraries(
    w3c_tracecontext_test ${CMAKE_THREAD_LIBS_INIT} opentelemetry_trace
    http_client_curl opentelemetry_exporter_ostream_span ${CURL_LIBRARIES})
endif()
