if(WITH_OTPROTOCOL)
  add_subdirectory(otlp)
endif()

add_subdirectory(ostream)
add_subdirectory(memory)

if(WITH_PROMETHEUS)
  add_subdirectory(prometheus)
endif()

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
