cmake_minimum_required(VERSION 3.10)
project(opentelemetry-cpp-sdk-install-test LANGUAGES CXX)
message(STATUS "Testing the sdk component install")
find_package(opentelemetry-cpp REQUIRED COMPONENTS sdk)

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(sdk_test ${INSTALL_TEST_SRC_DIR}/test_sdk.cc)
target_link_libraries(
  sdk_test
  PRIVATE opentelemetry-cpp::api
          opentelemetry-cpp::sdk
          opentelemetry-cpp::version
          opentelemetry-cpp::common
          opentelemetry-cpp::resources
          opentelemetry-cpp::trace
          opentelemetry-cpp::metrics
          opentelemetry-cpp::logs
          GTest::GTest
          GTest::Main)

gtest_discover_tests(sdk_test)
