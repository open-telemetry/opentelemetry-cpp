set(COMMON_SRCS random.cc core.cc)
if(WIN32)
  list(APPEND COMMON_SRCS platform/fork_windows.cc)
else()
  list(APPEND COMMON_SRCS platform/fork_unix.cc)
endif()

add_library(opentelemetry_common ${COMMON_SRCS})
target_link_libraries(opentelemetry_common Threads::Threads
                      ${CORE_RUNTIME_LIBS})
