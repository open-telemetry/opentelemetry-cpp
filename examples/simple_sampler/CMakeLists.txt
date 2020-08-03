add_library(testing_library testing_library/testing_library.cc)
target_link_libraries(testing_library ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)

add_executable(example_simple main.cc)
target_link_libraries(example_simple ${CMAKE_THREAD_LIBS_INIT} testing_library
                      opentelemetry_trace)
