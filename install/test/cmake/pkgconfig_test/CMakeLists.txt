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

add_executable(full_pkgconfig_test)

foreach(component IN LISTS INSTALL_TEST_COMPONENTS)
  message(STATUS "Adding test source for component ${component}")
  target_sources(full_pkgconfig_test
                 PRIVATE "${INSTALL_TEST_SRC_DIR}/test_${component}.cc")
endforeach()

target_include_directories(full_pkgconfig_test
                           PRIVATE ${_otel_pkgconfig_include_dirs})
target_compile_options(full_pkgconfig_test
                       PRIVATE ${_otel_pkgconfig_cflags_other})
target_link_libraries(
  full_pkgconfig_test
  PRIVATE ${_otel_pkgconfig_libraries} ${_otel_pkgconfig_ldflags} GTest::gtest
          GTest::gtest_main)
