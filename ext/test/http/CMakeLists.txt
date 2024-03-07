# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(WITH_HTTP_CLIENT_CURL)
  set(FILENAME curl_http_test)
  add_compile_definitions(WITH_CURL)
  add_executable(${FILENAME} ${FILENAME}.cc)
  target_link_libraries(${FILENAME} ${GTEST_BOTH_LIBRARIES}
                        ${CMAKE_THREAD_LIBS_INIT})

  if(TARGET CURL::libcurl)
    target_link_libraries(${FILENAME} opentelemetry_http_client_curl
                          opentelemetry_common CURL::libcurl)
  else()
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries(${FILENAME} ${CURL_LIBRARIES}
                          opentelemetry_http_client_curl opentelemetry_common)
  endif()
  gtest_add_tests(
    TARGET ${FILENAME}
    TEST_PREFIX ext.http.curl.
    TEST_LIST ${FILENAME})
endif()

set(URL_UTIL_FILENAME url_util_test)
add_executable(${URL_UTIL_FILENAME} ${URL_UTIL_FILENAME}.cc)
target_link_libraries(${URL_UTIL_FILENAME} ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT} opentelemetry_api)
gtest_add_tests(
  TARGET ${URL_UTIL_FILENAME}
  TEST_PREFIX ext.http.urlparser.
  TEST_LIST ${URL_UTIL_FILENAME})
