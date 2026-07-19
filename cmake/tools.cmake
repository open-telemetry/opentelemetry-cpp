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

function(project_build_tools_set_export_declaration OUTPUT_VARNAME)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang|Intel|XL|XLClang")
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
      set(${OUTPUT_VARNAME}
          "__attribute__((__dllexport__))"
          PARENT_SCOPE)
    else()
      set(${OUTPUT_VARNAME}
          "__attribute__((visibility(\"default\")))"
          PARENT_SCOPE)
    endif()
  elseif(MSVC)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
      set(${OUTPUT_VARNAME}
          "__declspec(dllexport)"
          PARENT_SCOPE)
    else()
      set(${OUTPUT_VARNAME}
          ""
          PARENT_SCOPE)
    endif()
  elseif(SunPro)
    set(${OUTPUT_VARNAME}
        "__global"
        PARENT_SCOPE)
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(${OUTPUT_VARNAME}
        "__declspec(dllexport)"
        PARENT_SCOPE)
  else()
    set(${OUTPUT_VARNAME}
        ""
        PARENT_SCOPE)
  endif()
endfunction()

function(project_build_tools_set_import_declaration OUTPUT_VARNAME)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang|Intel|XL|XLClang")
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
      set(${OUTPUT_VARNAME}
          "__attribute__((__dllimport__))"
          PARENT_SCOPE)
    else()
      set(${OUTPUT_VARNAME}
          ""
          PARENT_SCOPE)
    endif()
  elseif(MSVC)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
      set(${OUTPUT_VARNAME}
          "__declspec(dllimport)"
          PARENT_SCOPE)
    else()
      set(${OUTPUT_VARNAME}
          ""
          PARENT_SCOPE)
    endif()
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "SunPro")
    set(${OUTPUT_VARNAME}
        "__global"
        PARENT_SCOPE)
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(${OUTPUT_VARNAME}
        "__declspec(dllimport)"
        PARENT_SCOPE)
  else()
    set(${OUTPUT_VARNAME}
        ""
        PARENT_SCOPE)
  endif()
endfunction()

function(project_build_tools_set_shared_library_declaration DEFINITION_VARNAME)
  project_build_tools_set_export_declaration(EXPORT_DECLARATION)
  project_build_tools_set_import_declaration(IMPORT_DECLARATION)
  foreach(TARGET_NAME ${ARGN})
    target_compile_definitions(
      ${TARGET_NAME} INTERFACE "${DEFINITION_VARNAME}=${IMPORT_DECLARATION}")
    target_compile_definitions(
      ${TARGET_NAME} PRIVATE "${DEFINITION_VARNAME}=${EXPORT_DECLARATION}")
  endforeach()
endfunction()

function(project_build_tools_set_static_library_declaration DEFINITION_VARNAME)
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang|Intel|XL|XLClang")
    foreach(TARGET_NAME ${ARGN})
      target_compile_definitions(
        ${TARGET_NAME}
        PUBLIC "${DEFINITION_VARNAME}=__attribute__((visibility(\"default\")))")
    endforeach()
  else()
    foreach(TARGET_NAME ${ARGN})
      target_compile_definitions(${TARGET_NAME} PUBLIC "${DEFINITION_VARNAME}=")
    endforeach()
  endif()
endfunction()

function(_otelcpp_get_legacy_option_name __OPTION_NAME __OUTPUT_VARIABLE)
  if(__OPTION_NAME MATCHES "^OTELCPP_WITH_")
    string(REGEX REPLACE "^OTELCPP_" "" __LEGACY_OPTION_NAME "${__OPTION_NAME}")
  elseif(__OPTION_NAME STREQUAL "OTELCPP_BUILD_W3CTRACECONTEXT_TEST")
    set(__LEGACY_OPTION_NAME "BUILD_W3CTRACECONTEXT_TEST")
  elseif(__OPTION_NAME STREQUAL "OTELCPP_INSTALL")
    set(__LEGACY_OPTION_NAME "OPENTELEMETRY_INSTALL")
  elseif(__OPTION_NAME STREQUAL "OTELCPP_SKIP_DYNAMIC_LOADING_TESTS")
    set(__LEGACY_OPTION_NAME "OPENTELEMETRY_SKIP_DYNAMIC_LOADING_TESTS")
  elseif(__OPTION_NAME STREQUAL "OTELCPP_BUILD_DLL")
    set(__LEGACY_OPTION_NAME "OPENTELEMETRY_BUILD_DLL")
  elseif(__OPTION_NAME STREQUAL "OTELCPP_BUILD_PACKAGE")
    set(__LEGACY_OPTION_NAME "BUILD_PACKAGE")
  elseif(__OPTION_NAME STREQUAL "OTELCPP_EXTERNAL_COMPONENT_PATH")
    set(__LEGACY_OPTION_NAME "OPENTELEMETRY_EXTERNAL_COMPONENT_PATH")
  elseif(__OPTION_NAME STREQUAL "OTELCPP_TARBALL")
    set(__LEGACY_OPTION_NAME "TARBALL")
  else()
    set(__LEGACY_OPTION_NAME "")
  endif()

  set(${__OUTPUT_VARIABLE}
      "${__LEGACY_OPTION_NAME}"
      PARENT_SCOPE)
endfunction()

function(_otelcpp_get_option_default __OPTION_NAME __DEFAULT_VALUE
         __OUTPUT_VARIABLE)
  set(__RESOLVED_DEFAULT "${__DEFAULT_VALUE}")
  _otelcpp_get_legacy_option_name("${__OPTION_NAME}" __LEGACY_OPTION_NAME)

  if(NOT __LEGACY_OPTION_NAME STREQUAL "")
    if(DEFINED ${__LEGACY_OPTION_NAME})
      # Read the legacy value even when the namespaced option takes precedence,
      # so CMake does not report a recognized deprecated option as unused.
      set(__LEGACY_VALUE "${${__LEGACY_OPTION_NAME}}")
      message(
        DEPRECATION
          "CMake option ${__LEGACY_OPTION_NAME} is deprecated. Use ${__OPTION_NAME} instead."
      )
      # The namespaced option takes precedence when both names are provided.
      if(NOT DEFINED ${__OPTION_NAME})
        set(__RESOLVED_DEFAULT "${__LEGACY_VALUE}")
      endif()
    endif()
  endif()

  set(${__OUTPUT_VARIABLE}
      "${__RESOLVED_DEFAULT}"
      PARENT_SCOPE)
endfunction()

function(otelcpp_option_flag __OPTION_NAME __HELP_TEXT __DEFAULT_VALUE)
  _otelcpp_get_option_default("${__OPTION_NAME}" "${__DEFAULT_VALUE}"
                              __RESOLVED_DEFAULT)
  option(${__OPTION_NAME} "${__HELP_TEXT}" "${__RESOLVED_DEFAULT}")
endfunction()

function(otelcpp_option_variable __OPTION_NAME __DEFAULT_VALUE __CACHE_TYPE
         __DOC_STRING)
  _otelcpp_get_option_default("${__OPTION_NAME}" "${__DEFAULT_VALUE}"
                              __RESOLVED_DEFAULT)
  set(${__OPTION_NAME}
      "${__RESOLVED_DEFAULT}"
      CACHE "${__CACHE_TYPE}" "${__DOC_STRING}" ${ARGN})
endfunction()

include(CMakeDependentOption)

function(otelcpp_dependent_option __OPTION_NAME __HELP_TEXT
         __CONDITION_TRUE_VALUE __CONDITION_RULE __CONDITION_FALSE_VALUE)
  _otelcpp_get_option_default("${__OPTION_NAME}" "${__CONDITION_TRUE_VALUE}"
                              __RESOLVED_DEFAULT)
  cmake_dependent_option(
    ${__OPTION_NAME} "${__HELP_TEXT}" "${__RESOLVED_DEFAULT}"
    "${__CONDITION_RULE}" "${__CONDITION_FALSE_VALUE}")
  # cmake_dependent_option uses a normal variable when the condition is false.
  # Propagate it out of this wrapper function as well as retaining cache values.
  set(${__OPTION_NAME}
      "${${__OPTION_NAME}}"
      PARENT_SCOPE)
endfunction()
