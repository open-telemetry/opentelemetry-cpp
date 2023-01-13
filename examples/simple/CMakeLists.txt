add_definitions(-DOPENTELEMETRY_BUILD_IMPORT_DLL)

include_directories(${CMAKE_SOURCE_DIR}/exporters/ostream/include)


add_executable(example_simple main.cc)
target_link_libraries(
  example_simple ${CMAKE_THREAD_LIBS_INIT} common_foo_library
  opentelemetry_cpp)
