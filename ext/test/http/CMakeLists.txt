# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(NOT GMOCK_LIB AND TARGET GTest::gmock)
  set(GMOCK_LIB GTest::gmock)
elseif(MSVC)
  # Explicitly specify that we consume GTest from shared library. The rest of
  # code logic below determines whether we link Release or Debug flavor of the
  # library. These flavors have different prefix on Windows, gmock and gmockd
  # respectively.
  add_definitions(-DGTEST_LINKED_AS_SHARED_LIBRARY=1)
  if(GMOCK_LIB)
    # unset GMOCK_LIB to force find_library to redo the lookup, as the cached
    # entry could cause linking to incorrect flavor of gmock and leading to
    # runtime error.
    unset(GMOCK_LIB CACHE)
  endif()
endif()
if(NOT GMOCK_LIB)
  if(MSVC AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_library(GMOCK_LIB gmockd PATH_SUFFIXES lib)
  else()
    find_library(GMOCK_LIB gmock PATH_SUFFIXES lib)
  endif()
endif()

if(WITH_HTTP_CLIENT_CURL)
  set(FILENAME curl_http_test)
  add_compile_definitions(WITH_CURL)
  add_executable(${FILENAME} ${FILENAME}.cc)
  target_link_libraries(${FILENAME} ${GMOCK_LIB} ${GTEST_BOTH_LIBRARIES}
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

set(URL_PARSER_FILENAME url_parser_test)
add_executable(${URL_PARSER_FILENAME} ${URL_PARSER_FILENAME}.cc)
target_link_libraries(${URL_PARSER_FILENAME} opentelemetry_api ${GMOCK_LIB}
                      ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
gtest_add_tests(
  TARGET ${URL_PARSER_FILENAME}
  TEST_PREFIX ext.http.urlparser.
  TEST_LIST ${URL_PARSER_FILENAME})
