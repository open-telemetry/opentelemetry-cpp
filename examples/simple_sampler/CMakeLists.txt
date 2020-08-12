add_library(test_tracer test_tracer/test_tracer.cc)
target_link_libraries(test_tracer ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)

add_executable(example_simple main.cc)
target_link_libraries(example_simple ${CMAKE_THREAD_LIBS_INIT} test_tracer
                      opentelemetry_trace)
