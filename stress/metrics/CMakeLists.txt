# Define the metrics executable
add_executable(stress_metrics metrics.cc)

# Link throughput library and OpenTelemetry Metrics API
target_link_libraries(
  stress_metrics PRIVATE stress opentelemetry_metrics # OpenTelemetry Metrics
                                                      # SDK
)

# Include directories for throughput
target_include_directories(stress_metrics
                           PRIVATE ${CMAKE_SOURCE_DIR}/stress/common)

# Set properties
set_target_properties(
  stress_metrics
  PROPERTIES CXX_STANDARD 17
             CXX_STANDARD_REQUIRED YES
             CXX_EXTENSIONS NO)

# Optional: Installation
if(OPENTELEMETRY_INSTALL)
  install(
    TARGETS stress_metrics
    EXPORT "${PROJECT_NAME}-target"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
