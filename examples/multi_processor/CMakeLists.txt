include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include
                    ${CMAKE_SOURCE_DIR}/exporters/memory/include)

add_library(foo_multi_library foo_library/foo_library.cc)
target_link_libraries(foo_multi_library opentelemetry_exporter_ostream_span
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)

add_executable(example_multi_processor main.cc)
target_link_libraries(example_multi_processor ${CMAKE_THREAD_LIBS_INIT}
                      foo_multi_library opentelemetry_trace)
