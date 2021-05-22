include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include)

find_package(CURL)
if(NOT CURL_FOUND)
  message(WARNING "Skipping example_w3c_tracecontext_test: CURL not found")
else()
  find_package(nlohmann_json)
  if(NOT nlohmann_json_FOUND)
    add_subdirectory(${PROJECT_SOURCE_DIR}/third_party/nlohmann-json)
  endif()
  add_executable(w3c_tracecontext_test main.cc)
  target_link_libraries(
    w3c_tracecontext_test
    PRIVATE ${CMAKE_THREAD_LIBS_INIT} opentelemetry_trace http_client_curl
            opentelemetry_exporter_ostream_span ${CURL_LIBRARIES}
            nlohmann_json::nlohmann_json)
endif()
