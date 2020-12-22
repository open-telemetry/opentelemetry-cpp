include_directories(include)

add_library(opentelemetry_exporter_otprotocol src/recordable.cc
                                              src/otlp_exporter.cc)
target_link_libraries(opentelemetry_exporter_otprotocol opentelemetry_proto)

if(BUILD_TESTING)
  add_executable(recordable_test test/recordable_test.cc)
  target_link_libraries(
    recordable_test ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
    opentelemetry_exporter_otprotocol protobuf::libprotobuf gRPC::grpc++)
  gtest_add_tests(
    TARGET recordable_test
    TEST_PREFIX exporter.
    TEST_LIST recordable_test)
endif() # BUILD_TESTING
