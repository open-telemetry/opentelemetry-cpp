add_library(common_logs_foo_library foo_library.h foo_library.cc)
target_link_libraries(common_logs_foo_library PUBLIC ${CMAKE_THREAD_LIBS_INIT}
                                                     opentelemetry_api)
