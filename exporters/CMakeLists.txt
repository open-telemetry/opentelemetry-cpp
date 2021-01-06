if(WITH_OTLP)
  add_subdirectory(otlp)
endif()

add_subdirectory(ostream)
add_subdirectory(memory)

if(WITH_PROMETHEUS)
  add_subdirectory(prometheus)
endif()

if(WITH_ELASTICSEARCH)
  add_subdirectory(elasticsearch)
endif()

if(WITH_ETW)
  add_subdirectory(etw)
endif()
