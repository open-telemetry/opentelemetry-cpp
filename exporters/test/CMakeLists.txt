if(WITH_CURL)
  add_executable("curl_http_test" "curl_http_test.cc")
  include_directories(${CURL_INCLUDE_DIR})
  target_link_libraries("curl_http_test" ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} ${CURL_LIBRARIES})
  gtest_add_tests(TARGET "curl_http_test" TEST_PREFIX curl. TEST_LIST
                  "curl_http_test")
endif()
