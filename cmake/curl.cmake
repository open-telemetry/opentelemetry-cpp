# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

if(OPENTELEMETRY_INSTALL)
  set(_CURL_DISABLE_INSTALL OFF)
else()
  set(_CURL_DISABLE_INSTALL ON)
endif()

set(_OTEL_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

otel_add_thirdparty_package(
  PACKAGE_NAME "CURL"
  PACKAGE_SEARCH_MODES "MODULE" "CONFIG"
  FETCH_NAME "curl"
  FETCH_GIT_REPOSITORY "https://github.com/curl/curl.git"
  FETCH_GIT_TAG "${curl_GIT_TAG}"
  FETCH_CMAKE_ARGS
    CURL_DISABLE_INSTALL=${_CURL_DISABLE_INSTALL}
    CURL_USE_LIBPSL=OFF
    BUILD_CURL_EXE=OFF
    BUILD_LIBCURL_DOCS=OFF
    BUILD_MISC_DOCS=OFF
    ENABLE_CURL_MANUAL=OFF
    BUILD_SHARED_LIBS=ON
  VERSION_REGEX "#define[ \t]+LIBCURL_VERSION[ \t]+\"([0-9]+\\.[0-9]+\\.[0-9]+(-[A-Za-z0-9]+)?)\""
  VERSION_FILE "\${curl_SOURCE_DIR}/include/curl/curlver.h"
)

set(BUILD_SHARED_LIBS ${_OTEL_BUILD_SHARED_LIBS} CACHE BOOL "" FORCE)
unset(_OTEL_BUILD_SHARED_LIBS)
unset(_CURL_DISABLE_INSTALL)

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
