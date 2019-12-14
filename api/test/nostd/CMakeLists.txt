add_executable(string_view_test string_view_test.cc)
target_link_libraries(string_view_test catch2_with_main)
target_link_libraries(string_view_test opentelemetry_api)
add_test(NAME string_view_test COMMAND string_view_test)

include(GoogleTest)
add_executable(string_view_test2 string_view_test2.cc)
target_link_libraries(string_view_test2 ${GTEST_BOTH_LIBRARIES})
gtest_add_tests(
  TARGET string_view_test2 
  TEST_PREFIX nostd.
  TEST_LIST string_view_test2)
