set(COMMON_SRCS random.cc core.cc global_log_handler.cc)
if(WIN32)
  list(APPEND COMMON_SRCS platform/fork_windows.cc)
else()
  list(APPEND COMMON_SRCS platform/fork_unix.cc)
endif()

add_library(opentelemetry_common ${COMMON_SRCS})

set_target_properties(opentelemetry_common PROPERTIES EXPORT_NAME common)

target_link_libraries(
  opentelemetry_common PUBLIC opentelemetry_api opentelemetry_sdk
                              Threads::Threads)

install(
  TARGETS opentelemetry_common
  EXPORT "${PROJECT_NAME}-target"
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
