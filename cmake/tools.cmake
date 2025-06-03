# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

macro(check_append_cxx_compiler_flag OUTPUT_VAR)
  foreach(CHECK_FLAG ${ARGN})
    check_cxx_compiler_flag(${CHECK_FLAG}
                            "check_cxx_compiler_flag_${CHECK_FLAG}")
    if(check_cxx_compiler_flag_${CHECK_FLAG})
      list(APPEND ${OUTPUT_VAR} ${CHECK_FLAG})
    endif()
  endforeach()
endmacro()

if(NOT PATCH_PROTOBUF_SOURCES_OPTIONS_SET)
  if(MSVC)
    unset(PATCH_PROTOBUF_SOURCES_OPTIONS CACHE)
    set(PATCH_PROTOBUF_SOURCES_OPTIONS
        /wd4244
        /wd4251
        /wd4267
        /wd4309
        /wd4668
        /wd4946
        /wd6001
        /wd6244
        /wd6246)

    if(MSVC_VERSION GREATER_EQUAL 1922)
      # see
      # https://docs.microsoft.com/en-us/cpp/overview/cpp-conformance-improvements?view=vs-2019#improvements_162
      # for detail
      list(APPEND PATCH_PROTOBUF_SOURCES_OPTIONS /wd5054)
    endif()

    if(MSVC_VERSION GREATER_EQUAL 1925)
      list(APPEND PATCH_PROTOBUF_SOURCES_OPTIONS /wd4996)
    endif()

    if(MSVC_VERSION LESS 1910)
      list(APPEND PATCH_PROTOBUF_SOURCES_OPTIONS /wd4800)
    endif()
  else()
    unset(PATCH_PROTOBUF_SOURCES_OPTIONS CACHE)
    include(CheckCXXCompilerFlag)
    check_append_cxx_compiler_flag(
      PATCH_PROTOBUF_SOURCES_OPTIONS -Wno-type-limits
      -Wno-deprecated-declarations -Wno-unused-parameter)
  endif()
  set(PATCH_PROTOBUF_SOURCES_OPTIONS_SET TRUE)
  if(PATCH_PROTOBUF_SOURCES_OPTIONS)
    set(PATCH_PROTOBUF_SOURCES_OPTIONS
        ${PATCH_PROTOBUF_SOURCES_OPTIONS}
        CACHE INTERNAL
              "Options to disable warning of generated protobuf sources" FORCE)
  endif()
endif()

function(patch_protobuf_sources)
  if(PATCH_PROTOBUF_SOURCES_OPTIONS)
    foreach(PROTO_SRC ${ARGN})
      unset(PROTO_SRC_OPTIONS)
      get_source_file_property(PROTO_SRC_OPTIONS ${PROTO_SRC} COMPILE_OPTIONS)
      if(PROTO_SRC_OPTIONS)
        list(APPEND PROTO_SRC_OPTIONS ${PATCH_PROTOBUF_SOURCES_OPTIONS})
      else()
        set(PROTO_SRC_OPTIONS ${PATCH_PROTOBUF_SOURCES_OPTIONS})
      endif()

      set_source_files_properties(
        ${PROTO_SRC} PROPERTIES COMPILE_OPTIONS "${PROTO_SRC_OPTIONS}")
    endforeach()
    unset(PROTO_SRC)
    unset(PROTO_SRC_OPTIONS)
  endif()
endfunction()

function(patch_protobuf_targets)
  if(PATCH_PROTOBUF_SOURCES_OPTIONS)
    foreach(PROTO_TARGET ${ARGN})
      unset(PROTO_TARGET_OPTIONS)
      get_target_property(PROTO_TARGET_OPTIONS ${PROTO_TARGET} COMPILE_OPTIONS)
      if(PROTO_TARGET_OPTIONS)
        list(APPEND PROTO_TARGET_OPTIONS ${PATCH_PROTOBUF_SOURCES_OPTIONS})
      else()
        set(PROTO_TARGET_OPTIONS ${PATCH_PROTOBUF_SOURCES_OPTIONS})
      endif()

      set_target_properties(
        ${PROTO_TARGET} PROPERTIES COMPILE_OPTIONS "${PROTO_TARGET_OPTIONS}")
    endforeach()
    unset(PROTO_TARGET)
    unset(PROTO_TARGET_OPTIONS)
  endif()
endfunction()

function(project_build_tools_get_imported_location OUTPUT_VAR_NAME TARGET_NAME)

  # The following if statement was added to support cmake versions < 3.19
  get_target_property(TARGET_TYPE ${TARGET_NAME} TYPE)
  if(TARGET_TYPE STREQUAL "INTERFACE_LIBRARY")
    # For interface libraries, do not attempt to retrieve imported location.
    set(${OUTPUT_VAR_NAME} "" PARENT_SCOPE)
    return()
  endif()

  if(CMAKE_BUILD_TYPE)
    string(TOUPPER "IMPORTED_LOCATION_${CMAKE_BUILD_TYPE}"
                   TRY_SPECIFY_IMPORTED_LOCATION)
    get_target_property(${OUTPUT_VAR_NAME} ${TARGET_NAME}
                        ${TRY_SPECIFY_IMPORTED_LOCATION})
  endif()
  if(NOT ${OUTPUT_VAR_NAME})
    get_target_property(${OUTPUT_VAR_NAME} ${TARGET_NAME} IMPORTED_LOCATION)
  endif()
  if(NOT ${OUTPUT_VAR_NAME})
    get_target_property(
      project_build_tools_get_imported_location_IMPORTED_CONFIGURATIONS
      ${TARGET_NAME} IMPORTED_CONFIGURATIONS)
    foreach(
      project_build_tools_get_imported_location_IMPORTED_CONFIGURATION IN
      LISTS project_build_tools_get_imported_location_IMPORTED_CONFIGURATIONS)
      get_target_property(
        ${OUTPUT_VAR_NAME}
        ${TARGET_NAME}
        "IMPORTED_LOCATION_${project_build_tools_get_imported_location_IMPORTED_CONFIGURATION}"
      )
      if(${OUTPUT_VAR_NAME})
        break()
      endif()
    endforeach()
  endif()
  if(${OUTPUT_VAR_NAME})
    set(${OUTPUT_VAR_NAME}
        ${${OUTPUT_VAR_NAME}}
        PARENT_SCOPE)
  endif()
endfunction()

#[[
If we build third party packages with a different CONFIG setting from building
otel-cpp, cmake may not find a suitable file in imported targets (#705, #1359)
when linking. But on some platforms, different CONFIG settings can be used when
these CONFIG settings have the same ABI. For example, on Linux, we can build
gRPC and protobuf with -DCMAKE_BUILD_TYPE=Release, but build otel-cpp with
-DCMAKE_BUILD_TYPE=Debug and link these libraries together.
The properties of imported targets can be found here:
https://cmake.org/cmake/help/latest/manual/cmake-properties.7.html#properties-on-targets
]]
function(project_build_tools_patch_default_imported_config)
  set(PATCH_VARS
      IMPORTED_IMPLIB
      IMPORTED_LIBNAME
      IMPORTED_LINK_DEPENDENT_LIBRARIES
      IMPORTED_LINK_INTERFACE_LANGUAGES
      IMPORTED_LINK_INTERFACE_LIBRARIES
      IMPORTED_LINK_INTERFACE_MULTIPLICITY
      IMPORTED_LOCATION
      IMPORTED_NO_SONAME
      IMPORTED_OBJECTS
      IMPORTED_SONAME)
  foreach(TARGET_NAME ${ARGN})
    if(TARGET ${TARGET_NAME})
      get_target_property(IS_IMPORTED_TARGET ${TARGET_NAME} IMPORTED)
      if(NOT IS_IMPORTED_TARGET)
        continue()
      endif()

      get_target_property(IS_ALIAS_TARGET ${TARGET_NAME} ALIASED_TARGET)
      if(IS_ALIAS_TARGET)
        continue()
      endif()

      if(CMAKE_VERSION VERSION_LESS "3.19.0")
        get_target_property(TARGET_TYPE_NAME ${TARGET_NAME} TYPE)
        if(TARGET_TYPE_NAME STREQUAL "INTERFACE_LIBRARY")
          continue()
        endif()
      endif()

      get_target_property(DO_NOT_OVERWRITE ${TARGET_NAME} IMPORTED_LOCATION)
      if(DO_NOT_OVERWRITE)
        continue()
      endif()

      # MSVC's STL and debug level must match the target, so we can only move
      # out IMPORTED_LOCATION_NOCONFIG
      if(MSVC)
        set(PATCH_IMPORTED_CONFIGURATION "NOCONFIG")
      else()
        get_target_property(PATCH_IMPORTED_CONFIGURATION ${TARGET_NAME}
                            IMPORTED_CONFIGURATIONS)
      endif()

      if(NOT PATCH_IMPORTED_CONFIGURATION)
        continue()
      endif()

      get_target_property(PATCH_TARGET_LOCATION ${TARGET_NAME}
                          "IMPORTED_LOCATION_${PATCH_IMPORTED_CONFIGURATION}")
      if(NOT PATCH_TARGET_LOCATION)
        continue()
      endif()

      foreach(PATCH_IMPORTED_KEY IN LISTS PATCH_VARS)
        get_target_property(
          PATCH_IMPORTED_VALUE ${TARGET_NAME}
          "${PATCH_IMPORTED_KEY}_${PATCH_IMPORTED_CONFIGURATION}")
        if(PATCH_IMPORTED_VALUE)
          set_target_properties(
            ${TARGET_NAME} PROPERTIES "${PATCH_IMPORTED_KEY}"
                                      "${PATCH_IMPORTED_VALUE}")
          if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            message(
              STATUS
                "Patch: ${TARGET_NAME} ${PATCH_IMPORTED_KEY} will use ${PATCH_IMPORTED_KEY}_${PATCH_IMPORTED_CONFIGURATION}(\"${PATCH_IMPORTED_VALUE}\") by default."
            )
          endif()
        endif()
      endforeach()
    endif()
  endforeach()
endfunction()

#-------------------------------------------------------------------------------
# Set the target version for a given target
function(set_target_version target_name)
  if(OTELCPP_VERSIONED_LIBS)
    set_target_properties(
      ${target_name} PROPERTIES VERSION ${OPENTELEMETRY_VERSION}
                                SOVERSION ${OPENTELEMETRY_ABI_VERSION_NO})
  endif()
endfunction()

#-------------------------------------------------------------------------------
# otel_add_thirdparty_package
# Add a third party package to the project using CMake's find_package or FetchContent.
# By default the dependencies are found/fetched in the following order:
#   Step 1. Search for installed packages by calling find_package using the provided search modes.
#   Step 2. Use FetchContent on the provided source directory (used for git submodules)
#   Step 3. Use FetchContent to fetch the package from a git repository if not found in the previous steps.
# Arguments:
#   PACKAGE_NAME: The name of the package to find or fetch
#   PACKAGE_SEARCH_MODES: The cmake find_package search modes. Multiple search modes can be provided
#            (ie: SEARCH_MODES "MODULE" "CONFIG" will search first using the MODULE mode, then CONFIG mode).
#   FETCH_NAME: The name of the package to fetch if not found
#   FETCH_SOURCE_DIR: The directory to fetch the package into
#   FETCH_GIT_REPOSITORY: The git repository URL to fetch the package from
#   FETCH_GIT_TAG: The git tag to checkout
#   FETCH_CMAKE_ARGS: The cmake arguments used to build the package
#   REQUIRED_TARGETS: The targets to check for existence
#   VERSION_REGEX: The regex to parse the package version if fetched from source or git repository. Defaults to parsing the version from the third_party_release file.
#   VERSION_FILE: The file to read the version from when using VERSION_REGEX. Defaults to "opentelemetry-cpp/third_party_release".

# Output variables set at the parent scope:
#   <package>_SOURCE_DIR: Set if the package was fetched
#   <package>_BINARY_DIR: Set if the package was fetched
#   <package>_VERSION: The version of the package found or fetched
#   <package>_PROVIDER: The provider of the package (package, fetch_source, fetch_repository)
function(otel_add_thirdparty_package)

  set(optionArgs )
  set(oneValueArgs PACKAGE_NAME FETCH_NAME FETCH_GIT_REPOSITORY FETCH_GIT_TAG FETCH_SOURCE_DIR VERSION_REGEX VERSION_FILE)
  set(multiValueArgs PACKAGE_SEARCH_MODES FETCH_CMAKE_ARGS REQUIRED_TARGETS )
  cmake_parse_arguments(_THIRDPARTY "${optionArgs}" "${oneValueArgs}" "${multiValueArgs}" "${ARGN}")

  if(NOT _THIRDPARTY_PACKAGE_NAME)
    message(FATAL_ERROR "PACKAGE_NAME is required")
  endif()

  if(DEFINED _THIRDPARTY_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unparsed arguments detected: ${_THIRDPARTY_UNPARSED_ARGUMENTS}")
  endif()

  if(NOT _THIRDPARTY_FETCH_NAME)
    set(_THIRDPARTY_FETCH_NAME ${_THIRDPARTY_PACKAGE_NAME})
  endif()

  if(NOT _THIRDPARTY_VERSION_FILE)
    set(_THIRDPARTY_VERSION_FILE "${PROJECT_SOURCE_DIR}/third_party_release")
  endif()

  if(NOT _THIRDPARTY_VERSION_REGEX)
    set(_THIRDPARTY_VERSION_REGEX "${_THIRDPARTY_FETCH_NAME}=[ \\t]*v?([0-9]+(\\.[0-9]+)*)")
  endif()

  if(DEFINED _THIRDPARTY_FETCH_GIT_REPOSITORY AND NOT _THIRDPARTY_FETCH_GIT_TAG)
    message(FATAL_ERROR "FETCH_GIT_TAG is required if FETCH_GIT_REPOSITORY is defined")
  endif()

  message(STATUS "Adding third party package ${_THIRDPARTY_PACKAGE_NAME}")
  message(DEBUG "  Search modes: ${_THIRDPARTY_PACKAGE_SEARCH_MODES}")
  message(DEBUG "  Fetch name: ${_THIRDPARTY_FETCH_NAME}")
  message(DEBUG "  Source dir: ${_THIRDPARTY_FETCH_SOURCE_DIR}")
  message(DEBUG "  Git repository: ${_THIRDPARTY_FETCH_GIT_REPOSITORY}")
  message(DEBUG "  Git tag: ${_THIRDPARTY_FETCH_GIT_TAG}")
  message(DEBUG "  CMake args: ${_THIRDPARTY_FETCH_CMAKE_ARGS}")
  message(DEBUG "  Required targets: ${_THIRDPARTY_REQUIRED_TARGETS}")
  message(DEBUG "  Version regex: ${_THIRDPARTY_VERSION_REGEX}")
  message(DEBUG "  Version file: ${_THIRDPARTY_VERSION_FILE}")

  if(NOT OTELCPP_FORCE_FETCH_DEPENDENCIES)
    foreach(_search_mode IN LISTS _THIRDPARTY_PACKAGE_SEARCH_MODES)
      message(STATUS "  Searching for ${_THIRDPARTY_PACKAGE_NAME} with search mode ${_search_mode}")
      find_package(${_THIRDPARTY_PACKAGE_NAME} ${_search_mode} QUIET)
      if(${_THIRDPARTY_PACKAGE_NAME}_FOUND)
        # ZLIB and CURL have a version string in older versions of cmake. Set <package>_VERSION from <package>_VERSION_STRING
        if(DEFINED ${_THIRDPARTY_PACKAGE_NAME}_VERSION_STRING AND NOT DEFINED ${_THIRDPARTY_PACKAGE_NAME}_VERSION)
          set(${_THIRDPARTY_PACKAGE_NAME}_VERSION ${${_THIRDPARTY_PACKAGE_NAME}_VERSION_STRING})
        endif()
        message(STATUS "  Found ${_THIRDPARTY_PACKAGE_NAME} with search mode ${_search_mode}")
        break()
      endif()
    endforeach()
  endif()

  if(${_THIRDPARTY_PACKAGE_NAME}_FOUND)
    set("${_THIRDPARTY_PACKAGE_NAME}_PROVIDER" "package")
  elseif(DEFINED _THIRDPARTY_FETCH_SOURCE_DIR OR DEFINED _THIRDPARTY_FETCH_GIT_REPOSITORY)
    # Use FetchContent to fetch the package if not found
    include(FetchContent)

    if(DEFINED _THIRDPARTY_FETCH_SOURCE_DIR AND EXISTS "${_THIRDPARTY_FETCH_SOURCE_DIR}/.git")
      message(STATUS "  Fetching ${_THIRDPARTY_FETCH_PACKAGE_NAME} from local source at ${_THIRDPARTY_FETCH_SOURCE_DIR}")
      FetchContent_Declare(
          ${_THIRDPARTY_FETCH_NAME}
          SOURCE_DIR ${_THIRDPARTY_FETCH_SOURCE_DIR}
      )
      set("${_THIRDPARTY_PACKAGE_NAME}_PROVIDER" "fetch_source")
    elseif( NOT OTELCPP_REQUIRE_LOCAL_DEPENDENCIES AND DEFINED _THIRDPARTY_FETCH_GIT_REPOSITORY AND DEFINED _THIRDPARTY_FETCH_GIT_TAG)
      message(STATUS "  Fetching ${_THIRDPARTY_PACKAGE_NAME} from ${_THIRDPARTY_FETCH_GIT_REPOSITORY} at tag ${_THIRDPARTY_FETCH_GIT_TAG}")
      FetchContent_Declare(
          ${_THIRDPARTY_FETCH_NAME}
          GIT_REPOSITORY
          ${_THIRDPARTY_FETCH_GIT_REPOSITORY}
          GIT_TAG
          ${_THIRDPARTY_FETCH_GIT_TAG}
          GIT_SHALLOW ON
      )
      set("${_THIRDPARTY_PACKAGE_NAME}_PROVIDER" "fetch_repository")
    else()
      message(FATAL_ERROR "No valid source found for ${_THIRDPARTY_PACKAGE_NAME}")
    endif()

    # Set the CMake arguments (KEY=VALUE) for the third party package
    foreach(_arg IN LISTS _THIRDPARTY_FETCH_CMAKE_ARGS)
      if(_arg MATCHES "^([^=]+)=(.*)$")
        set(_key   "${CMAKE_MATCH_1}")
        set(_value "${CMAKE_MATCH_2}")
        message(DEBUG "  Setting ${_key}=${_value}")
        set(${_key} "${_value}" CACHE STRING "" FORCE)
      else()
        message(WARNING "  ignoring malformed CMAKE_ARG: ${_arg}")
      endif()
    endforeach()

    set(SAVED_CMAKE_CXX_CLANG_TIDY ${CMAKE_CXX_CLANG_TIDY})
    set(SAVED_CMAKE_CXX_INCLUDE_WHAT_YOU_USE
          ${CMAKE_CXX_INCLUDE_WHAT_YOU_USE})
    set(CMAKE_CXX_CLANG_TIDY "")
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "")

    FetchContent_MakeAvailable(${_THIRDPARTY_FETCH_NAME})

    set(CMAKE_CXX_CLANG_TIDY ${SAVED_CMAKE_CXX_CLANG_TIDY})
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE
        ${SAVED_CMAKE_CXX_INCLUDE_WHAT_YOU_USE})

    set(${_THIRDPARTY_FETCH_NAME}_SOURCE_DIR ${${_THIRDPARTY_FETCH_NAME}_SOURCE_DIR} PARENT_SCOPE)
    set(${_THIRDPARTY_FETCH_NAME}_BINARY_DIR ${${_THIRDPARTY_FETCH_NAME}_BINARY_DIR} PARENT_SCOPE)
  endif()

  # Check for required targets
  foreach(_target ${_THIRDPARTY_REQUIRED_TARGETS})
    if(NOT TARGET ${_target})
      message(FATAL_ERROR "Required target ${_target} not found")
    endif()
  endforeach()

  # Determine the package version if not already defined
  if(NOT DEFINED ${_THIRDPARTY_PACKAGE_NAME}_VERSION AND NOT ${_THIRDPARTY_PACKAGE_NAME}_PROVIDER STREQUAL "package")
    if(DEFINED _THIRDPARTY_VERSION_REGEX AND DEFINED _THIRDPARTY_VERSION_FILE)
      string(CONFIGURE "${_THIRDPARTY_VERSION_FILE}" THIRDPARTY_VERSION_FILE)
      if(NOT EXISTS "${THIRDPARTY_VERSION_FILE}")
        message(WARNING "Version file ${THIRDPARTY_VERSION_FILE} does not exist")
      else()
        file(READ "${THIRDPARTY_VERSION_FILE}" _file_contents)
        string(REGEX MATCH
              "${_THIRDPARTY_VERSION_REGEX}"
              _version_match
              "${_file_contents}")
        if(_version_match)
          set("${_THIRDPARTY_PACKAGE_NAME}_VERSION" "${CMAKE_MATCH_1}")
        else()
          message(WARNING "Failed to parse version from ${_THIRDPARTY_VERSION_FILE} using regex ${_THIRDPARTY_VERSION_REGEX}")
        endif()
      endif()
    endif()
  endif()

  message(STATUS "${_THIRDPARTY_PACKAGE_NAME} version: ${${_THIRDPARTY_PACKAGE_NAME}_VERSION}")
  message(STATUS "${_THIRDPARTY_PACKAGE_NAME} provider: ${${_THIRDPARTY_PACKAGE_NAME}_PROVIDER}")

  set(${_THIRDPARTY_PACKAGE_NAME}_VERSION ${${_THIRDPARTY_PACKAGE_NAME}_VERSION} PARENT_SCOPE)
  set(${_THIRDPARTY_PACKAGE_NAME}_PROVIDER ${${_THIRDPARTY_PACKAGE_NAME}_PROVIDER} PARENT_SCOPE)
endfunction()
