include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include)

add_library(foo_library foo_library/foo_library.cc)
target_link_libraries(foo_library opentelemetry_exporter_ostream_span
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)

add_executable(example_simple main.cc)
target_link_libraries(example_simple ${CMAKE_THREAD_LIBS_INIT} foo_library
                      opentelemetry_trace opentelemetry_resources)
