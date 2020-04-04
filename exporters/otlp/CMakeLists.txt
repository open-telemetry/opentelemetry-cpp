add_library(opentelemetry_exporter_otprotocol recordable.cc)
target_link_libraries(opentelemetry_exporter_otprotocol
                      $<TARGET_OBJECTS:opentelemetry_proto>)
