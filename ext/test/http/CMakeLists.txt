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

# The CMake target is POSIX-only (Windows has no SIGPIPE); the Bazel target
# covers Windows. The platform-dependent tests are declared unconditionally in
# the POSIX source and skip at runtime where their mechanism is unavailable, so
# gtest_add_tests, which scans the source, registers only tests the POSIX binary
# actually contains.
if(NOT WIN32)
  set(SOCKET_SIGPIPE_FILENAME socket_sigpipe_test)
  add_executable(${SOCKET_SIGPIPE_FILENAME} ${SOCKET_SIGPIPE_FILENAME}.cc)
  target_link_libraries(
    ${SOCKET_SIGPIPE_FILENAME} opentelemetry_ext ${GMOCK_LIB}
    ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
  gtest_add_tests(
    TARGET ${SOCKET_SIGPIPE_FILENAME}
    TEST_PREFIX ext.http.socketsigpipe.
    TEST_LIST ${SOCKET_SIGPIPE_FILENAME})
endif()

set(URL_PARSER_FILENAME url_parser_test)
add_executable(${URL_PARSER_FILENAME} ${URL_PARSER_FILENAME}.cc)
target_link_libraries(${URL_PARSER_FILENAME} opentelemetry_ext ${GMOCK_LIB}
                      ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
gtest_add_tests(
  TARGET ${URL_PARSER_FILENAME}
  TEST_PREFIX ext.http.urlparser.
  TEST_LIST ${URL_PARSER_FILENAME})
