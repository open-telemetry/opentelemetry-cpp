add_library(opentelemetry_exporter_otprotocol recordable.cc)
target_link_libraries(opentelemetry_exporter_otprotocol
                      $<TARGET_OBJECTS:opentelemetry_proto>)

add_executable(recordable_test "recordable_test.cc")
target_link_libraries(recordable_test ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_exporters)
gtest_add_tests(TARGET recordable_test TEST_PREFIX exporters. TEST_LIST recordable_test)
