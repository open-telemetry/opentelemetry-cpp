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
    set(${OUTPUT_VAR_NAME}
        ""
        PARENT_SCOPE)
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

function(__project_build_tools_recursive_scan_unwrap OUTPUT_VAR INPUT_VAR)
  # With sub-expressions
  if(INPUT_VAR MATCHES "^\\\$<.*>:([^>]+)>$")
    set(${OUTPUT_VAR}
        "${CMAKE_MATCH_1}"
        PARENT_SCOPE)
  elseif(INPUT_VAR MATCHES "^\\\$<.*>:([^>]+)$")
    set(${OUTPUT_VAR}
        "${CMAKE_MATCH_1}"
        PARENT_SCOPE)
  elseif(INPUT_VAR MATCHES "^\\\$<[^:]+:([^>]+)>$")
    set(${OUTPUT_VAR}
        "${CMAKE_MATCH_1}"
        PARENT_SCOPE)
  elseif(INPUT_VAR MATCHES "^\\\$<[^:]+:([^>]+)$")
    set(${OUTPUT_VAR}
        "${CMAKE_MATCH_1}"
        PARENT_SCOPE)
  else()
    set(${OUTPUT_VAR}
        "${INPUT_VAR}"
        PARENT_SCOPE)
  endif()
endfunction()

function(__project_build_tools_unwrap_interface_link_libraries OUTPUT_VAR)
  set(optionArgs)
  set(oneValueArgs)
  set(multiValueArgs TARGET_NAME TARGET_MATCH)
  cmake_parse_arguments(__SCAN_OPTION "${optionArgs}" "${oneValueArgs}"
                        "${multiValueArgs}" "${ARGN}")
  set(__OUTPUT_VAR)

  set(__TARGET_CHECK_NAMES)
  foreach(TARGET_NAME_ORIGIN ${__SCAN_OPTION_TARGET_NAME})
    if(TARGET_NAME_ORIGIN MATCHES "^\\\$<.*>?$")
      __project_build_tools_recursive_scan_unwrap(TARGET_NAME
                                                  "${TARGET_NAME_ORIGIN}")
    else()
      set(TARGET_NAME "${TARGET_NAME_ORIGIN}")
    endif()

    if(TARGET "${TARGET_NAME}")
      # Get the imported location of the target
      project_build_tools_get_imported_location(__TARGET_IMPORTED_LOCATION
                                                "${TARGET_NAME}")
      if(__TARGET_IMPORTED_LOCATION)
        list(APPEND __TARGET_CHECK_NAMES ${__TARGET_IMPORTED_LOCATION})
      endif()

      # Get the interface link libraries of the target
      set(__TARGET_LINK_LIBRARIES)
      get_target_property(__TARGET_LINK_LIBRARIES "${TARGET_NAME}"
                          INTERFACE_LINK_LIBRARIES)
      list(APPEND __TARGET_CHECK_NAMES ${__TARGET_LINK_LIBRARIES})
    else()
      # If the name is not a target, it should be a link name. We can directly
      # append it to the checking list.
      list(APPEND __TARGET_CHECK_NAMES "${TARGET_NAME}")
    endif()
  endforeach()

  list(REMOVE_DUPLICATES __TARGET_CHECK_NAMES)
  # If the target match option is set, filter the link names
  if(__SCAN_OPTION_TARGET_MATCH)
    foreach(__LINK_NAME IN LISTS __TARGET_CHECK_NAMES)
      set(__TARGET_MATCHED FALSE)
      foreach(__MATCH_RULE ${__SCAN_OPTION_TARGET_MATCH})
        if(__LINK_NAME MATCHES "${__MATCH_RULE}")
          set(__TARGET_MATCHED TRUE)
          break()
        endif()
      endforeach()
      if(__TARGET_MATCHED)
        list(APPEND __OUTPUT_VAR ${__LINK_NAME})
      endif()
    endforeach()
  else()
    set(__OUTPUT_VAR ${__TARGET_CHECK_NAMES})
  endif()

  set(${OUTPUT_VAR}
      ${__OUTPUT_VAR}
      PARENT_SCOPE)
endfunction()

function(project_build_tools_unwrap_interface_link_libraries OUTPUT_VAR)
  set(optionArgs)
  set(oneValueArgs)
  set(multiValueArgs TARGET_NAME TARGET_MATCH)
  cmake_parse_arguments(__SCAN_OPTION "${optionArgs}" "${oneValueArgs}"
                        "${multiValueArgs}" "${ARGN}")

  set(__OUTPUT_VAR_LEVEL_1)
  set(__OUTPUT_VAR_LEVEL_2)
  __project_build_tools_unwrap_interface_link_libraries(__OUTPUT_VAR_LEVEL_1
                                                        "${ARGN}")
  __project_build_tools_unwrap_interface_link_libraries(
    __OUTPUT_VAR_LEVEL_2 TARGET_NAME ${__OUTPUT_VAR_LEVEL_1} TARGET_MATCH
    ${__SCAN_OPTION_TARGET_MATCH})

  set(${OUTPUT_VAR}
      ${__OUTPUT_VAR_LEVEL_2}
      PARENT_SCOPE)
endfunction()
