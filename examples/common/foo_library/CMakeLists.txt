add_definitions(-DOPENTELEMETRY_BUILD_IMPORT_DLL)

add_library(common_foo_library foo_library.h foo_library.cc)
target_link_libraries(common_foo_library PUBLIC ${CMAKE_THREAD_LIBS_INIT}
                                                opentelemetry_api)
