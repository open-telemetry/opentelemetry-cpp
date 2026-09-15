# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.16)

project(opentelemetry-cpp-pkgconfig-install-test LANGUAGES CXX)

if(NOT DEFINED INSTALL_TEST_SRC_DIR)
  message(
    FATAL_ERROR
      "INSTALL_TEST_SRC_DIR must be defined when running cmake on this test project"
  )
endif()

if(NOT INSTALL_TEST_COMPONENTS)
  message(FATAL_ERROR "INSTALL_TEST_COMPONENTS is empty")
endif()

find_package(PkgConfig REQUIRED)
find_package(GTest CONFIG REQUIRED)

set(_pkgconfig_pc_files "")
foreach(_prefix IN LISTS CMAKE_PREFIX_PATH)
  file(GLOB _prefix_pkgconfig_pc_files
       "${_prefix}/lib/pkgconfig/opentelemetry_*.pc")
  list(APPEND _pkgconfig_pc_files ${_prefix_pkgconfig_pc_files})
endforeach()
if(NOT _pkgconfig_pc_files)
  message(
    FATAL_ERROR
      "No installed opentelemetry pkg-config files found under ${CMAKE_PREFIX_PATH}/lib/pkgconfig"
  )
endif()

set(_pkgconfig_modules "")
foreach(_pc_file IN LISTS _pkgconfig_pc_files)
  get_filename_component(_module "${_pc_file}" NAME_WE)
  list(APPEND _pkgconfig_modules "${_module}")
endforeach()
list(SORT _pkgconfig_modules)

message(STATUS "Testing pkg-config install on modules = ${_pkgconfig_modules}")
message(
  STATUS "Testing pkg-config install on components = ${INSTALL_TEST_COMPONENTS}"
)

pkg_check_modules(OTEL_PKGCONFIG REQUIRED ${_pkgconfig_modules})

# CMake 3.16 has no STATIC keyword on pkg_check_modules(), but it also
# queries pkg-config --static and exposes the results as <prefix>_STATIC_*.
# Install tests pass BUILD_SHARED_LIBS through INSTALL_TEST_CMAKE_OPTIONS.
if(BUILD_SHARED_LIBS)
  set(_otel_pkgconfig_include_dirs ${OTEL_PKGCONFIG_INCLUDE_DIRS})
  set(_otel_pkgconfig_cflags_other ${OTEL_PKGCONFIG_CFLAGS_OTHER})
  set(_otel_pkgconfig_libraries ${OTEL_PKGCONFIG_LIBRARIES})
  set(_otel_pkgconfig_ldflags ${OTEL_PKGCONFIG_LDFLAGS})
else()
  set(_otel_pkgconfig_include_dirs ${OTEL_PKGCONFIG_STATIC_INCLUDE_DIRS})
  set(_otel_pkgconfig_cflags_other ${OTEL_PKGCONFIG_STATIC_CFLAGS_OTHER})
  set(_otel_pkgconfig_libraries ${OTEL_PKGCONFIG_STATIC_LIBRARIES})
  set(_otel_pkgconfig_ldflags ${OTEL_PKGCONFIG_STATIC_LDFLAGS})
endif()

# INSTALL_TEST_COMPONENTS is the CMake component set, not the set of components
# represented by installed .pc files. Compile a test source only when this
# component owns at least one discovered opentelemetry_*.pc module. Ownership is
# the first argument of opentelemetry_add_pkgconfig() (module name
# opentelemetry_<name>). Components with no .pc files, such as resource
# detectors, are omitted.
function(_otel_pkgconfig_component_is_covered component modules result_var)
  set(_covered FALSE)
  set(_exact "")
  set(_families "")

  if(component STREQUAL "api")
    set(_families api)
  elseif(component STREQUAL "sdk")
    set(_families
        common
        logs
        metrics
        resources
        trace
        version
        instrumentation_scope
        configuration_core
        configuration_trace_builders
        configuration_logs_builders
        configuration_metrics_builders
        configuration_registry_factory)
  elseif(component STREQUAL "configuration")
    # Exact names only: a "configuration" family prefix would also match SDK
    # configuration_* modules that belong to the sdk component.
    set(_exact opentelemetry_configuration opentelemetry_configuration_yaml)
  elseif(component STREQUAL "ext_http_curl")
    set(_families http_client_curl)
  elseif(component STREQUAL "exporters_in_memory")
    set(_families exporter_in_memory)
  elseif(component STREQUAL "exporters_ostream")
    set(_families exporter_ostream)
  elseif(component STREQUAL "exporters_otlp_common")
    set(_families otlp_common otlp_recordable exporter_otlp_builder_utils proto)
  elseif(component STREQUAL "exporters_otlp_http")
    set(_families exporter_otlp_http)
  elseif(component STREQUAL "exporters_otlp_grpc")
    set(_families exporter_otlp_grpc)
  elseif(component STREQUAL "exporters_otlp_file")
    set(_families exporter_otlp_file)
  elseif(component STREQUAL "exporters_prometheus")
    set(_families exporter_prometheus)
  elseif(component STREQUAL "exporters_zipkin")
    set(_families exporter_zipkin)
  elseif(component STREQUAL "exporters_elasticsearch")
    set(_families exporter_elasticsearch)
  endif()

  foreach(_module IN LISTS modules)
    if(_module IN_LIST _exact)
      set(_covered TRUE)
    endif()
    foreach(_family IN LISTS _families)
      if(_module STREQUAL "opentelemetry_${_family}"
         OR _module MATCHES "^opentelemetry_${_family}_")
        set(_covered TRUE)
      endif()
    endforeach()
  endforeach()

  set(${result_var}
      ${_covered}
      PARENT_SCOPE)
endfunction()

add_executable(full_pkgconfig_test)

set(_pkgconfig_test_sources "")
foreach(component IN LISTS INSTALL_TEST_COMPONENTS)
  _otel_pkgconfig_component_is_covered(${component} "${_pkgconfig_modules}"
                                       _component_covered)
  if(NOT _component_covered)
    message(
      STATUS
        "Skipping pkg-config test source for component ${component} (no installed .pc coverage)"
    )
  else()
    set(_component_source "${INSTALL_TEST_SRC_DIR}/test_${component}.cc")
    if(NOT EXISTS "${_component_source}")
      message(
        FATAL_ERROR
          "Pkg-config coverage for component ${component} but missing test source ${_component_source}"
      )
    endif()
    message(STATUS "Adding test source for component ${component}")
    list(APPEND _pkgconfig_test_sources "${_component_source}")
    target_sources(full_pkgconfig_test PRIVATE "${_component_source}")
  endif()
endforeach()

if(NOT _pkgconfig_test_sources)
  message(
    FATAL_ERROR
      "No pkg-config test sources selected from INSTALL_TEST_COMPONENTS=${INSTALL_TEST_COMPONENTS}"
  )
endif()

target_include_directories(full_pkgconfig_test
                           PRIVATE ${_otel_pkgconfig_include_dirs})
target_compile_options(full_pkgconfig_test
                       PRIVATE ${_otel_pkgconfig_cflags_other})
target_link_libraries(
  full_pkgconfig_test
  PRIVATE ${_otel_pkgconfig_libraries} ${_otel_pkgconfig_ldflags} GTest::gtest
          GTest::gtest_main)
