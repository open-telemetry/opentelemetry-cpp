if (WITH_OTPROTOCOL)
  include(proto/opentelemetry/Protobuf.cmake)
endif()

add_subdirectory(src)
