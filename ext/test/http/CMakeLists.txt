find_package(CURL)
if(CURL_FOUND)
  set(FILENAME curl_http_test)
  add_compile_definitions(WITH_CURL)
  add_executable(${FILENAME} ${FILENAME}.cc)
  target_link_libraries(${FILENAME} ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT})

  if(TARGET CURL::libcurl)
    target_link_libraries(${FILENAME} CURL::libcurl opentelemetry_curl_factory)
  else()
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries(${FILENAME} ${CURL_LIBRARIES} opentelemetry_curl_factory)
  endif()
  gtest_add_tests(
    TARGET ${FILENAME}
    TEST_PREFIX ext.http.curl.
    TEST_LIST ${FILENAME})
endif()
