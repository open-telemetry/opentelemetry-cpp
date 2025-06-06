# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

#-----------------------------------------------------------------------
# otel_parse_version_from_file
#   Parses the version from a file using a regex.
#  Arguments:
#    REGEX: The regex pattern to match the version.
#    FILE: The file to read the version from.
#    OUT_VERSION: The variable to store the parsed version.
#  Returns:
#    OUT_VERSION: The parsed version string.
#-----------------------------------------------------------------------
function(otel_parse_version_from_file)
  set(optionArgs )
  set(oneValueArgs REGEX FILE OUT_VERSION)
  set(multiValueArgs)
  cmake_parse_arguments(_OTEL_PARSE "${optionArgs}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

  if(DEFINED _OTEL_PARSE_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unparsed arguments detected: ${_OTEL_PARSE_UNPARSED_ARGUMENTS}")
  endif()

  if(NOT DEFINED _OTEL_PARSE_OUT_VERSION)
    message(FATAL_ERROR "OUT_VERSION is required")
  endif()
  if(NOT DEFINED _OTEL_PARSE_REGEX)
    message(FATAL_ERROR "REGEX is required")
  endif()
  if(NOT DEFINED _OTEL_PARSE_FILE)
    message(FATAL_ERROR "FILE is required")
  endif()

  string(CONFIGURE "${_OTEL_PARSE_FILE}" _CONFIGURED_VERSION_FILEPATH)
  if(NOT EXISTS "${_CONFIGURED_VERSION_FILEPATH}")
    message(WARNING "Version file ${_CONFIGURED_VERSION_FILEPATH} does not exist")
  else()
    file(READ "${_CONFIGURED_VERSION_FILEPATH}" _VERSION_FILE_CONTENTS)
    string(REGEX MATCH
          "${_OTEL_PARSE_REGEX}"
          _VERSION_MATCH
          "${_VERSION_FILE_CONTENTS}")
    if(_VERSION_MATCH)
      set("${_OTEL_PARSE_OUT_VERSION}" "${CMAKE_MATCH_1}" PARENT_SCOPE)
    else()
      message(WARNING "Failed to parse version from ${_OTEL_PARSE_FILE} using regex ${_OTEL_PARSE_REGEX}")
    endif()
  endif()
endfunction()

#-------------------------------------------------------------------------------
# otel_get_project_version
#   Retrieves the OpenTelemetry C++ version from the version header file.
#  Arguments:
#    OTEL_SRC_DIR: The source directory of OpenTelemetry C++.
#    OUT_VERSION: The variable to store the parsed version.
#  Returns:
#    OUT_VERSION: The parsed OpenTelemetry C++ version.
#-------------------------------------------------------------------------------
function(otel_get_project_version OTEL_SRC_DIR OUT_VERSION)
  set(_OTEL_VERSION_FILE "${OTEL_SRC_DIR}/api/include/opentelemetry/version.h")
  set(_OTELCPP_VERSION_REGEX
      "OPENTELEMETRY_VERSION[ \t\r\n]+\"?([^\"]+)\"?")

  otel_parse_version_from_file(
    REGEX "${_OTELCPP_VERSION_REGEX}"
    FILE "${_OTEL_VERSION_FILE}"
    OUT_VERSION _otelcpp_version_parsed
  )

  if(NOT DEFINED _otelcpp_version_parsed)
      message(
      FATAL_ERROR
        "OPENTELEMETRY_VERSION not found on ${_OTEL_VERSION_FILE}"
    )
  endif()
  set(${OUT_VERSION} "${_otelcpp_version_parsed}" PARENT_SCOPE)
  message(STATUS "OpenTelemetry C++ version: ${_otelcpp_version_parsed}")
endfunction()

#-------------------------------------------------------------------------------
# otel_get_abi_version_number
#   Retrieves the OpenTelemetry C++ ABI version number from the api version header file.
#  Arguments:
#    OTEL_SRC_DIR: The source directory of OpenTelemetry C++.
#    OUT_ABI_NUMBER: The variable to store the parsed ABI number.
#  Returns:
#    OUT_ABI_NUMBER: The parsed OpenTelemetry C++ ABI number.
#-------------------------------------------------------------------------------
function(otel_get_abi_version_number OTEL_SRC_DIR OUT_ABI_NUMBER)
  set(_OTEL_VERSION_FILE "${OTEL_SRC_DIR}/api/include/opentelemetry/version.h")
  otel_parse_version_from_file(
    REGEX "OPENTELEMETRY_ABI_VERSION_NO[ \t\r\n]+\"?([0-9]+)\"?"
    FILE "${_OTEL_VERSION_FILE"
    OUT_VERSION _otel_abi_number_parsed)

  math(EXPR _otel_abi_number ${_otel_abi_number_parsed})
  if(NOT _otel_abi_number)
    message(
      FATAL_ERROR
        "OPENTELEMETRY_ABI_VERSION_NO not found on ${_OTEL_VERSION_FILE}"
    )
  endif()
  set(${OUT_ABI_NUMBER} "${_otel_abi_number}" PARENT_SCOPE)
  message(STATUS "OpenTelemetry C++ ABI version number: ${_otel_abi_number}")
endfunction()

#-------------------------------------------------------------------------------
# otel_get_thirdparty_version_tags
#   Retrieves the version tags for third-party dependencies from the version file.
#  Arguments:
#    OTEL_SRC_DIR: The source directory of OpenTelemetry C++.
#  Returns:
#    <package>_GIT_TAG: Sets the git tag variables for each third-party package that the parent scope
#-------------------------------------------------------------------------------
function(otel_get_thirdparty_version_tags OTEL_SRC_DIR)
  set(_OTEL_THIRDPARTY_TAG_FILE "${OTEL_SRC_DIR}/third_party_release")
  if(NOT EXISTS "${_OTEL_THIRDPARTY_TAG_FILE}")
    message(FATAL_ERROR "Third-party version file ${_OTEL_THIRDPARTY_TAG_FILE} was not found")
  endif()

  file(STRINGS "${_OTEL_THIRDPARTY_TAG_FILE}" _file_lines)

  foreach(_raw_line IN LISTS _file_lines)
    # Strip leading/trailing whitespace
    string(STRIP "${_raw_line}" _line)

    # Skip empty lines and comments
    if(_line STREQUAL "")
      continue()
    endif()
    if(_line MATCHES "^#")
      continue()
    endif()

    # Match "package_name=git_tag"
    if(_line MATCHES "^([^=]+)=(.+)$")
      set(_third_party_name   "${CMAKE_MATCH_1}")
      set(_git_tag  "${CMAKE_MATCH_2}")
      set(_third_party_tag_var "${_third_party_name}_GIT_TAG")
      set("${_third_party_tag_var}" "${_git_tag}" PARENT_SCOPE)
      message(STATUS "Set third-party tag: ${_third_party_tag_var}=${_git_tag}")
    else()
      message(WARNING "Skipping invalid line in ${_OTEL_THIRDPARTY_TAG_FILE}. Line:\n  ${_line}")
    endif()
  endforeach()
endfunction()
