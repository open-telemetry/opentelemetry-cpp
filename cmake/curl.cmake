# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(OPENTELEMETRY_INSTALL)
  set(_CURL_DISABLE_INSTALL OFF)
else()
  set(_CURL_DISABLE_INSTALL ON)
endif()

otel_add_thirdparty_package(
  PACKAGE_NAME "CURL"
  PACKAGE_SEARCH_MODES "CONFIG" "MODULE"
  FETCH_NAME "curl"
  FETCH_GIT_REPOSITORY "https://github.com/curl/curl.git"
  FETCH_GIT_TAG "${curl_GIT_TAG}"
  FETCH_CMAKE_ARGS
    "-DCURL_DISABLE_INSTALL=${_CURL_DISABLE_INSTALL}"
    "-DCURL_USE_LIBPSL=OFF"
    "-DBUILD_CURL_EXE=OFF"
    "-DBUILD_LIBCURL_DOCS=OFF"
    "-DBUILD_MISC_DOCS=OFF"
    "-DENABLE_CURL_MANUAL=OFF"
    "-DBUILD_SHARED_LIBS=ON"
  VERSION_REGEX "#define[ \t]+LIBCURL_VERSION[ \t]+\"([0-9]+\\.[0-9]+\\.[0-9]+(-[A-Za-z0-9]+)?)\""
  VERSION_FILE "\${curl_SOURCE_DIR}/include/curl/curlver.h"
)

if(NOT TARGET CURL::libcurl)
  if(TARGET libcurl_shared)
    add_library(CURL::libcurl ALIAS libcurl_shared)
  elseif(TARGET libcurl_static)
    add_library(CURL::libcurl ALIAS libcurl_static)
  endif()
endif()

if(NOT TARGET CURL::libcurl)
  message(FATAL_ERROR "CURL::libcurl imported target not found.")
endif()
