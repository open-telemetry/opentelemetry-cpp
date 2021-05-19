add_library(opentelemetry_exporter_otprotocol src/recordable.cc
                                              src/otlp_exporter.cc)

set_target_properties(opentelemetry_exporter_otprotocol
                      PROPERTIES EXPORT_NAME otlp_exporter)

target_link_libraries(
  opentelemetry_exporter_otprotocol
  PUBLIC opentelemetry_trace opentelemetry_resources opentelemetry_proto
         protobuf::libprotobuf gRPC::grpc++)

install(
  TARGETS opentelemetry_exporter_otprotocol
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY include/opentelemetry/exporters/otlp
  DESTINATION include/opentelemetry/exporters/
  FILES_MATCHING
  PATTERN "*.h")

target_include_directories(
  opentelemetry_exporter_otprotocol
  PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>")

if(BUILD_TESTING)
  add_executable(recordable_test test/recordable_test.cc)
  target_link_libraries(
    recordable_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    opentelemetry_exporter_otprotocol)
  gtest_add_tests(
    TARGET recordable_test
    TEST_PREFIX exporter.otlp.
    TEST_LIST recordable_test)
  if(MSVC)
    add_definitions(-DGTEST_LINKED_AS_SHARED_LIBRARY=1)
  else()
    if(GMOCK_LIB)
      # unset GMOCK_LIB to force find_library to redo the lookup, as the cached
      # entry could cause linking to incorrect flavor of gmock and leading to
      # runtime error.
      unset(GMOCK_LIB CACHE)
    endif()
  endif()
  if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_library(GMOCK_LIB gmockd PATH_SUFFIXES lib)
  else()
    find_library(GMOCK_LIB gmock PATH_SUFFIXES lib)
  endif()
  add_executable(otlp_exporter_test test/otlp_exporter_test.cc)
  target_link_libraries(
    otlp_exporter_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    ${GMOCK_LIB} opentelemetry_exporter_otprotocol)
  gtest_add_tests(
    TARGET otlp_exporter_test
    TEST_PREFIX exporter.otlp.
    TEST_LIST otlp_exporter_test)
endif() # BUILD_TESTING
