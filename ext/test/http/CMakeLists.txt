# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(WITH_HTTP_CLIENT_CURL)
  set(FILENAME curl_http_test)
  add_executable(${FILENAME} ${FILENAME}.cc)
  target_link_libraries(
    ${FILENAME}
    PRIVATE ${GMOCK_LIB} ${GTEST_BOTH_LIBRARIES} opentelemetry_http_client_curl
            opentelemetry_common CURL::libcurl)
  gtest_add_tests(
    TARGET ${FILENAME}
    TEST_PREFIX ext.http.curl.
    TEST_LIST ${FILENAME})
endif()

set(SOCKET_TOOLS_FILENAME socket_tools_test)
add_executable(${SOCKET_TOOLS_FILENAME} ${SOCKET_TOOLS_FILENAME}.cc)
target_link_libraries(${SOCKET_TOOLS_FILENAME} opentelemetry_ext ${GMOCK_LIB}
                      ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
if(WIN32)
  # socket_tools.h autolinks ws2_32 via #pragma comment(lib) under MSVC only; name it
  # explicitly so the test also links under MinGW/GCC where that pragma is ignored.
  target_link_libraries(${SOCKET_TOOLS_FILENAME} ws2_32)
endif()
gtest_add_tests(
  TARGET ${SOCKET_TOOLS_FILENAME}
  TEST_PREFIX ext.http.sockettools.
  TEST_LIST ${SOCKET_TOOLS_FILENAME})

set(URL_PARSER_FILENAME url_parser_test)
add_executable(${URL_PARSER_FILENAME} ${URL_PARSER_FILENAME}.cc)
target_link_libraries(${URL_PARSER_FILENAME} opentelemetry_ext ${GMOCK_LIB}
                      ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
gtest_add_tests(
  TARGET ${URL_PARSER_FILENAME}
  TEST_PREFIX ext.http.urlparser.
  TEST_LIST ${URL_PARSER_FILENAME})
