if(WITH_OTLP)
  add_subdirectory(otlp)
endif()

add_subdirectory(ostream)
add_subdirectory(memory)
add_subdirectory(elasticsearch)

if(WITH_PROMETHEUS)
  add_subdirectory(prometheus)
endif()
