add_library(opentelemetry_logs logger_provider.cc logger.cc
                               simple_log_processor.cc batch_log_processor.cc)

target_link_libraries(opentelemetry_logs opentelemetry_common)
