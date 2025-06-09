# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

set(OTELCPP_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
get_filename_component(OTELCPP_SOURCE_DIR "${OTELCPP_SOURCE_DIR}" ABSOLUTE)
set(OTELCPP_API_VERSION_FILE "${OTELCPP_SOURCE_DIR}/api/include/opentelemetry/version.h")

file(READ "${OTELCPP_API_VERSION_FILE}"
     OPENTELEMETRY_CPP_HEADER_VERSION_H)

if(NOT DEFINED OPENTELEMETRY_CPP_HEADER_VERSION_H)
      message(FATAL_ERROR "${OTELCPP_API_VERSION_FILE} not found")
endif()

if(OPENTELEMETRY_CPP_HEADER_VERSION_H MATCHES
   "OPENTELEMETRY_VERSION[ \t\r\n]+\"?([^\"]+)\"?")
  set(OPENTELEMETRY_VERSION ${CMAKE_MATCH_1})
else()
  message(
    FATAL_ERROR
      "OPENTELEMETRY_VERSION not found on ${OTELCPP_API_VERSION_FILE}"
 )
endif()

if(OPENTELEMETRY_CPP_HEADER_VERSION_H MATCHES "OPENTELEMETRY_ABI_VERSION_NO[ \t\r\n]+\"?([0-9]+)\"?")
   math(EXPR OPENTELEMETRY_ABI_VERSION_DEFAULT ${CMAKE_MATCH_1})
endif()

if(NOT OPENTELEMETRY_VERSION OR OPENTELEMETRY_VERSION VERSION_LESS_EQUAL "0.0.0")
  message(FATAL_ERROR "Failed to extract OpenTelemetry C++ version (${OPENTELEMETRY_VERSION}) from ${OTELCPP_API_VERSION_FILE}")
endif()

if(NOT OPENTELEMETRY_ABI_VERSION_DEFAULT)
  message(FATAL_ERROR "Failed to extract OpenTelemetry C++ ABI version from ${OTELCPP_API_VERSION_FILE}")
endif()

message(STATUS "OpenTelemetry C++ version: ${OPENTELEMETRY_VERSION}")

# Allow overriding the third-party version tags file with -DOTELCPP_THIRDPARTY_FILE=<file>
if(NOT OTELCPP_THIRDPARTY_FILE)
  set(OTELCPP_THIRDPARTY_FILE "${OTELCPP_SOURCE_DIR}/third_party_release")
elseif(NOT IS_ABSOLUTE OTELCPP_THIRDPARTY_FILE )
  string(PREPEND OTELCPP_THIRDPARTY_FILE "${OTELCPP_SOURCE_DIR}/")
endif()

if(NOT EXISTS "${OTELCPP_THIRDPARTY_FILE}")
  message(FATAL_ERROR "Third-party version tags file not found: ${OTELCPP_THIRDPARTY_FILE}")
endif()

message(STATUS "Reading third-party version tags from ${OTELCPP_THIRDPARTY_FILE}")

file(STRINGS "${OTELCPP_THIRDPARTY_FILE}" OTELCPP_THIRDPARTY_FILE_CONTENT)

set(OTELCPP_THIRDPARTY_PACKAGE_LIST "")
# Parse the third-party tags file
foreach(_raw_line IN LISTS OTELCPP_THIRDPARTY_FILE_CONTENT)
    # Strip leading/trailing whitespace
    string(STRIP "${_raw_line}" _line)
    # Skip empty lines and comments
    if(_line STREQUAL "" OR _line MATCHES "^#")
      continue()
    endif()

    # Match "package_name=git_tag"
    if(_line MATCHES "^([^=]+)=(.+)$")
      set(_third_party_name   "${CMAKE_MATCH_1}")
      set(_git_tag  "${CMAKE_MATCH_2}")
      set("${_third_party_name}_GIT_TAG" "${_git_tag}")
      list(APPEND OTELCPP_THIRDPARTY_PACKAGE_LIST "${_third_party_name}")
    else()
      message(FATAL_ERROR "Could not parse third-party tag. Invalid line in ${OTELCPP_THIRDPARTY_FILE}. Line:\n  ${_raw_line}")
    endif()
endforeach()
