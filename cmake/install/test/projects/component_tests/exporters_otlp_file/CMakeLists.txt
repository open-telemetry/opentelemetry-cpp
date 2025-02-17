cmake_minimum_required(VERSION 3.10)
project(opentelemetry-cpp-exporters-otlp-file-install-test LANGUAGES CXX)
message(STATUS "Testing the exporters_otlp_file_test component install")
find_package(opentelemetry-cpp REQUIRED COMPONENTS exporters_otlp_file)

if(NOT TARGET protobuf::libprotobuf)
  message(FATAL_ERROR "protobuf::libprotobuf target not found")
endif()

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(exporters_otlp_file_test
               ${INSTALL_TEST_SRC_DIR}/test_exporters_otlp_file.cc)
target_link_libraries(
  exporters_otlp_file_test
  PRIVATE opentelemetry-cpp::otlp_file_client
          opentelemetry-cpp::otlp_file_exporter
          opentelemetry-cpp::otlp_file_log_record_exporter
          opentelemetry-cpp::otlp_file_metric_exporter
          GTest::GTest
          GTest::Main)

gtest_discover_tests(exporters_otlp_file_test)
