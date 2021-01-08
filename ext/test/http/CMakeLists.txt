find_package(CURL)
if(CURL_FOUND)
  set(CURL_LIBRARY "-lcurl")
  set(FILENAME curl_http_test)
  add_compile_definitions(WITH_CURL)
  add_executable(${FILENAME} ${FILENAME}.cc)
  include_directories(${CURL_INCLUDE_DIR})
  target_link_libraries(
    ${FILENAME} ${GTEST_BOTH_LIBRARIES} opentelemetry_curl_factory
    ${CMAKE_THREAD_LIBS_INIT} ${CURL_LIBRARIES})
  gtest_add_tests(
    TARGET ${FILENAME}
    TEST_PREFIX ext.http.curl.
    TEST_LIST ${FILENAME})
endif()
