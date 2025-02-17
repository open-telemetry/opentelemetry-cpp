cmake_minimum_required(VERSION 3.10)

project(opentelemetry-cpp-full-package-install-test LANGUAGES CXX)

if(NOT DEFINED INSTALL_TEST_SRC_DIR)
  message(
    FATAL_ERROR
      "INSTALL_TEST_SRC_DIR must be defined when running cmake on this test project"
  )
endif()

find_package(opentelemetry-cpp CONFIG REQUIRED)

# Test that the following variables are set: OPENTELEMETRY_CPP_INCLUDE_DIRS  -
# Include directories of opentelemetry-cpp. OPENTELEMETRY_CPP_LIBRARY_DIRS  -
# Link directories of opentelemetry-cpp. OPENTELEMETRY_CPP_LIBRARIES     - List
# of libraries when using opentelemetry-cpp. OPENTELEMETRY_CPP_FOUND         -
# True if opentelemetry-cpp found. OPENTELEMETRY_ABI_VERSION_NO    - ABI version
# of opentelemetry-cpp. OPENTELEMETRY_VERSION           - Version of
# opentelemetry-cpp. OPENTELEMETRY_CPP_COMPONENTS_INSTALLED - List of components
# installed.

if(NOT OPENTELEMETRY_CPP_INCLUDE_DIRS)
  message(FATAL_ERROR "OPENTELEMETRY_CPP_INCLUDE_DIRS is empty")
endif()

if(NOT OPENTELEMETRY_CPP_LIBRARY_DIRS)
  message(FATAL_ERROR "OPENTELEMETRY_CPP_LIBRARY_DIRS is empty")
endif()

if(NOT OPENTELEMETRY_CPP_LIBRARIES)
  message(FATAL_ERROR "OPENTELEMETRY_CPP_LIBRARIES is empty")
endif()

if(NOT OPENTELEMETRY_CPP_FOUND)
  message(FATAL_ERROR "OPENTELEMETRY_CPP_FOUND is not set")
endif()

if(NOT OPENTELEMETRY_ABI_VERSION_NO)
  message(FATAL_ERROR "OPENTELEMETRY_ABI_VERSION_NO is empty")
endif()

if(NOT OPENTELEMETRY_VERSION)
  message(FATAL_ERROR "OPENTELEMETRY_VERSION is empty")
endif()

if(NOT opentelemetry-cpp_FOUND)
  message(
    FATAL_ERROR
      "calling find_package must import the components and set opentelemetry-cpp_FOUND"
  )
endif()

if(NOT OPENTELEMETRY_CPP_COMPONENTS_INSTALLED)
  message(FATAL_ERROR "OPENTELEMETRY_CPP_COMPONENTS_INSTALLED is empty")
endif()

# Set the components to test. If not set, use all installed components.
if(NOT COMPONENTS_TO_TEST)
  message(
    STATUS
      "COMPONENTS_TO_TEST is empty. Setting to OPENTELEMETRY_CPP_COMPONENTS_INSTALLED"
  )
  set(COMPONENTS_TO_TEST ${OPENTELEMETRY_CPP_COMPONENTS_INSTALLED})
endif()

message(
  STATUS
    "Testing the full package install on components = ${COMPONENTS_TO_TEST}")

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(full_test)

foreach(component IN LISTS COMPONENTS_TO_TEST)
  message(STATUS "Adding test source for component ${component}")
  target_sources(full_test
                 PRIVATE "${INSTALL_TEST_SRC_DIR}/test_${component}.cc")
endforeach()

target_include_directories(full_test PRIVATE ${OPENTELEMETRY_CPP_INCLUDE_DIRS})
target_link_libraries(full_test PRIVATE ${OPENTELEMETRY_CPP_LIBRARIES}
                                        GTest::GTest GTest::Main)

gtest_discover_tests(full_test)
