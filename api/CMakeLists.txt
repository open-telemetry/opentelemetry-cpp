add_library(opentelemetry_api INTERFACE)
target_include_directories(
  opentelemetry_api
  INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
            "$<INSTALL_INTERFACE:include>")

set_target_properties(opentelemetry_api PROPERTIES EXPORT_NAME api)

install(
  TARGETS opentelemetry_api
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

set(LOGS_EXCLUDE_PATTERN "")
if(NOT WITH_LOGS_PREVIEW)
  set(LOGS_EXCLUDE_PATTERN "logs")
endif()

set(METRICS_EXCLUDE_PATTERN "")
if(NOT WITH_METRICS_PREVIEW)
  set(METRICS_EXCLUDE_PATTERN "metrics")
endif()

install(
  DIRECTORY include/opentelemetry
  DESTINATION include
  FILES_MATCHING
  PATTERN "*.h"
  PATTERN "${METRICS_EXCLUDE_PATTERN}" EXCLUDE)

install(
  DIRECTORY include/opentelemetry
  DESTINATION include
  FILES_MATCHING
  PATTERN "*.h"
  PATTERN "${LOGS_EXCLUDE_PATTERN}" EXCLUDE)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

if(WITH_ABSEIL)

  find_package(absl CONFIG REQUIRED)

  target_compile_definitions(opentelemetry_api INTERFACE HAVE_ABSEIL)
  target_link_libraries(
    opentelemetry_api INTERFACE absl::bad_variant_access absl::any absl::base
                                absl::bits absl::city)

endif()

if(WITH_STL)
  message("Building with standard library types...")
  target_compile_definitions(opentelemetry_api INTERFACE HAVE_CPP_STDLIB)
else()
  message("Building with nostd types...")
endif()

if(WITH_GSL)
  target_compile_definitions(opentelemetry_api INTERFACE HAVE_GSL)

  # Guidelines Support Library path. Used if we are not on not get C++20.
  #
  find_package(Microsoft.GSL QUIET)
  if(TARGET Microsoft.GSL::GSL)
    target_link_libraries(opentelemetry_api INTERFACE Microsoft.GSL::GSL)
  else()
    set(GSL_DIR third_party/ms-gsl)
    target_include_directories(
      opentelemetry_api INTERFACE "$<BUILD_INTERFACE:${GSL_DIR}/include>")
  endif()
endif()

if(WITH_METRICS_PREVIEW)
  target_compile_definitions(opentelemetry_api INTERFACE ENABLE_METRICS_PREVIEW)
endif()

if(WITH_LOGS_PREVIEW)
  target_compile_definitions(opentelemetry_api INTERFACE ENABLE_LOGS_PREVIEW)
endif()

if(WITH_NO_GENENV)
  target_compile_definitions(opentelemetry_api INTERFACE NO_GETENV)
endif()

if(WIN32)
  target_compile_definitions(opentelemetry_api INTERFACE NOMINMAX)
  if(WITH_ETW)
    target_compile_definitions(opentelemetry_api INTERFACE HAVE_MSGPACK)
  endif()
endif()
