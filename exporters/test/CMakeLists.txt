if(WITH_CURL)
  set(FILENAME curl_http_test)
  add_executable(${FILENAME} ${FILENAME}.cc)
  include_directories(${CURL_INCLUDE_DIR})
  target_link_libraries(${FILENAME} ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} ${CURL_LIBRARIES})
  gtest_add_tests(TARGET ${FILENAME} TEST_PREFIX curl. TEST_LIST
                  ${FILENAME})
endif()