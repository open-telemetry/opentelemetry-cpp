add_library(opentelemetry_api INTERFACE)
target_include_directories(
  opentelemetry_api INTERFACE include
                              $<INSTALL_INTERFACE:include/opentelemetry>)

install(
  DIRECTORY include/opentelemetry
  DESTINATION include
  FILES_MATCHING
  PATTERN "*.h")

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

if(WITH_STL)
  message("Building with standard library types...")
else()
  message("Building with nostd types...")
endif()
