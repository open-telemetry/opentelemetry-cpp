add_executable(example_simple main.cc library.cc)
target_link_libraries(example_simple ${CMAKE_THREAD_LIBS_INIT}
                      opentelemetry_trace)
