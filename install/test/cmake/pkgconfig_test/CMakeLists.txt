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

add_executable(full_pkgconfig_test)

foreach(component IN LISTS INSTALL_TEST_COMPONENTS)
  message(STATUS "Adding test source for component ${component}")
  target_sources(full_pkgconfig_test
                 PRIVATE "${INSTALL_TEST_SRC_DIR}/test_${component}.cc")
endforeach()

target_include_directories(full_pkgconfig_test
                           PRIVATE ${OTEL_PKGCONFIG_INCLUDE_DIRS})
target_compile_options(full_pkgconfig_test
                       PRIVATE ${OTEL_PKGCONFIG_CFLAGS_OTHER})
target_link_libraries(
  full_pkgconfig_test
  PRIVATE ${OTEL_PKGCONFIG_LIBRARIES} ${OTEL_PKGCONFIG_LDFLAGS} GTest::gtest
          GTest::gtest_main)
