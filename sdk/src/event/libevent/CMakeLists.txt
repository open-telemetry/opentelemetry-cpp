add_library(opentelemetry_event_libevent io_dispatcher.cc event.cc event_base.cc
                                         file_event.cc timer.cc)
target_link_libraries(opentelemetry_event_libevent ${LIBEVENT_LIBRARIES})
