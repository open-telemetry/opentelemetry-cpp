find_package(CURL)
if(CURL_FOUND)
  set(CURL_LIBRARY "-lcurl")
  set(FILENAME curl_http_test)
  add_compile_definitions(WITH_CURL)

  add_executable(${FILENAME} ${FILENAME}.cc)
  include_directories(${CURL_INCLUDE_DIR})
  target_link_libraries(${FILENAME} ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT} ${CURL_LIBRARIES})
  gtest_add_tests(TARGET ${FILENAME} TEST_PREFIX ext.http.curl. TEST_LIST
                  ${FILENAME})
endif()
set(URL_PARSER_FILENAME url_parser_test)
add_executable(${URL_PARSER_FILENAME} ${URL_PARSER_FILENAME}.cc)
target_link_libraries(${URL_PARSER_FILENAME} ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
gtest_add_tests(TARGET ${URL_PARSER_FILENAME} TEST_PREFIX ext.http.urlparser. TEST_LIST
                  ${URL_PARSER_FILENAME})