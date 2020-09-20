if(WITH_OTPROTOCOL)
  add_subdirectory(otlp)
endif()

add_subdirectory(ostream)

if(WITH_PROMETHEUS)
  add_subdirectory(prometheus)
endif()
