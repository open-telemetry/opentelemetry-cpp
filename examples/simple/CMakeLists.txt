add_library(foo_library SHARED foo_library/foo_library.cc)
target_link_libraries(foo_library ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)

add_executable(example_simple main.cc)
target_link_libraries(example_simple ${CMAKE_THREAD_LIBS_INIT}
                      opentelemetry_trace foo_library)
