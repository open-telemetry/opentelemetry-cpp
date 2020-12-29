find_package(CURL)
if(CURL_FOUND)
  set(CURL_LIBRARY "-lcurl")
  set(FILENAME curl_http_test)
  add_executable(${FILENAME} ${FILENAME}.cc)
  target_link_libraries(${FILENAME} ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT})

  if(TARGET CURL::libcurl)
    target_link_libraries(${FILENAME} CURL::libcurl)
  else()
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries(${FILENAME} ${CURL_LIBRARIES})
  endif()
  gtest_add_tests(
    TARGET ${FILENAME}
    TEST_PREFIX ext.http.curl.
    TEST_LIST ${FILENAME})
endif()
