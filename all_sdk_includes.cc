#include <opentelemetry/sdk/metrics/meter_provider.h>
#include <opentelemetry/sdk/metrics/exemplar/fixed_size_exemplar_reservoir.h>
#include <opentelemetry/sdk/metrics/exemplar/no_exemplar_reservoir.h>

#include <opentelemetry/sdk/logs/logger_provider.h>
#include <opentelemetry/sdk/logs/logger_provider_factory.h>
#include <opentelemetry/sdk/logs/event_logger_provider.h>
#include <opentelemetry/sdk/logs/event_logger_provider_factory.h>
#include <opentelemetry/sdk/logs/simple_log_record_processor.h>
#include <opentelemetry/sdk/logs/batch_log_record_processor.h>
#include <opentelemetry/sdk/logs/simple_log_record_processor.h>
#include <opentelemetry/sdk/logs/simple_log_record_processor_factory.h>

#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/sdk/trace/simple_processor_factory.h>
#include <opentelemetry/sdk/trace/tracer_provider_factory.h>
#include <opentelemetry/sdk/trace/batch_span_processor_factory.h>

#include <optional>

#if defined(_MSC_VER)
#include <opentelemetry/exporters/etw/etw_logger.h>
#include <opentelemetry/exporters/etw/etw_tracer.h>
#endif

void OPENTELEMETRY_EXPORT otel_sdk_all_sdk_includes()
{
    using namespace opentelemetry::sdk;
    {
        [[maybe_unused]] std::optional<metrics::MeterProvider> t0;
        [[maybe_unused]] std::optional<metrics::FixedSizeExemplarReservoir> t1;
        [[maybe_unused]] std::optional<metrics::NoExemplarReservoir> t2;
        metrics::ExemplarReservoir::GetNoExemplarReservoir();
    }
    {
        [[maybe_unused]] std::optional<logs::LoggerProvider> t0;
        [[maybe_unused]] std::optional<logs::EventLoggerProvider> t1;
        [[maybe_unused]] std::optional<logs::SimpleLogRecordProcessor> t2;
        [[maybe_unused]] std::optional<logs::BatchLogRecordProcessor> t3;
        logs::EventLoggerProviderFactory::Create();
        std::unique_ptr<logs::LoggerContext> t4;
        logs::LoggerProviderFactory::Create(std::move(t4));
        std::unique_ptr<logs::LogRecordExporter> t5;
        logs::SimpleLogRecordProcessorFactory::Create(std::move(t5));
    }
    {
        trace::SimpleSpanProcessorFactory::Create({});
        [[maybe_unused]] std::unique_ptr<trace::SpanProcessor> t0;
        trace::TracerProviderFactory::Create(std::move(t0)); 
    }
}
