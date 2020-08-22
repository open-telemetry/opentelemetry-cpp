add_library(
  opentelemetry_zpages
  src/tracez_processor.cc
  src/tracez_data_aggregator.cc
  src/tracez_http_server.cc
)

target_include_directories(opentelemetry_zpages PUBLIC include)

target_link_libraries(opentelemetry_zpages opentelemetry_api
                      opentelemetry_trace http_server)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
