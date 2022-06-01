add_library(opentelemetry_sdk INTERFACE)
target_include_directories(
  opentelemetry_sdk
  INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
            "$<INSTALL_INTERFACE:include>")

set_target_properties(opentelemetry_sdk PROPERTIES EXPORT_NAME sdk)

install(
  TARGETS opentelemetry_sdk
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(
  DIRECTORY include/opentelemetry/
  DESTINATION include/opentelemetry
  FILES_MATCHING
  PATTERN "*config.h")

set(LOGS_EXCLUDE_PATTERN "")
if(NOT WITH_LOGS_PREVIEW)
  set(LOGS_EXCLUDE_PATTERN "logs")
endif()

set(METRICS_EXCLUDE_PATTERN "")
if(NOT WITH_METRICS_PREVIEW)
  set(METRICS_EXCLUDE_PATTERN "_metrics")
endif()

install(
  DIRECTORY include/opentelemetry/sdk
  DESTINATION include/opentelemetry
  FILES_MATCHING
  PATTERN "*.h"
  PATTERN "${METRICS_EXCLUDE_PATTERN}" EXCLUDE
  PATTERN "${LOGS_EXCLUDE_PATTERN}" EXCLUDE)

add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
