#include "otel_sdk.h"

#include <Windows.h>
#include <random>
#include <cstring>
#include <unordered_set>
#include <limits>

#pragma warning( push )
#pragma warning( disable : 4251 4275 4505 )

// logs, trace,  metrics
#include <opentelemetry/logs/provider.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/metrics/provider.h>

#include <opentelemetry/sdk/common/global_log_handler.h>
#include <opentelemetry/sdk/common/env_variables.h>

#include <opentelemetry/sdk/resource/semantic_conventions.h>

#include <opentelemetry/sdk/logs/logger_provider_factory.h>
#include <opentelemetry/sdk/logs/simple_log_record_processor_factory.h>
#include <opentelemetry/sdk/logs/batch_log_record_processor_options.h>
#include <opentelemetry/sdk/logs/batch_log_record_processor_factory.h>
#include <opentelemetry/sdk/logs/processor.h>

#include <opentelemetry/sdk/metrics/meter_provider_factory.h>
#include <opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h>
#include <opentelemetry/sdk/metrics/view/instrument_selector_factory.h>
#include <opentelemetry/sdk/metrics/view/meter_selector_factory.h>
#include <opentelemetry/sdk/metrics/view/view_factory.h>

#include <opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h>

#include "opentelemetry/exporters/ostream/log_record_exporter.h"
#include "opentelemetry/exporters/ostream/span_exporter_factory.h"
#include "opentelemetry/exporters/ostream/metric_exporter_factory.h"

#pragma warning( pop )

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace metrics
{
class DroppingAttributesProcessor : public AttributesProcessor
{
public:
	DroppingAttributesProcessor(
		const std::unordered_set<std::string> dropped_attribute_keys = {} )
		: dropped_attribute_keys_( std::move( dropped_attribute_keys ) )
	{
	}

	MetricAttributes process(
		const opentelemetry::common::KeyValueIterable &attributes ) const noexcept override
	{
		MetricAttributes result;
		attributes.ForEachKeyValue(
			[&]( nostd::string_view key, opentelemetry::common::AttributeValue value ) noexcept
			{
				if ( dropped_attribute_keys_.find( key.data() ) == dropped_attribute_keys_.cend() )
					result.SetAttribute( key, value );
				return true;
			} );
		return result;
	}

	bool isPresent( nostd::string_view key ) const noexcept override
	{
		return ( dropped_attribute_keys_.find( key.data() ) == dropped_attribute_keys_.end() );
	}

private:
	std::unordered_set<std::string> dropped_attribute_keys_;
};
} // namespace metrics
} // namespace sdk
OPENTELEMETRY_END_NAMESPACE

namespace otel_sdk
{
using namespace opentelemetry;

static bool extraDebug = false;

// Lifted from ToolsCore, as the goal of this library is to be independent.
// Later it'll be used outside in other CT tools.
static bool MatchWildcard( const char *str, const char *pat )
{
	const char *str_bt{ nullptr };
	const char *pat_bt{ nullptr };
	while ( *str != '\0' )
	{
		if ( *pat == '*' )
		{
			str_bt = str;
			pat_bt = ++pat;
		}
		else if ( *pat == '?' || *pat == *str )
		{
			str++;
			pat++;
		}
		else
		{
			if ( pat_bt == nullptr )
			{
				return false;
			}
			str = ++str_bt;
			pat = pat_bt;
		}
	}
	while ( *pat == '*' )
	{
		pat++;
	}
	return *pat == '\0';
}

class IgnoringVectoringExceptionHandler
{
	const PVOID exceptionHandlerHandle{};
	static LONG NTAPI exceptionHandler( PEXCEPTION_POINTERS /* exception */ ) noexcept
	{
		return EXCEPTION_CONTINUE_EXECUTION;
	}

public:
	IgnoringVectoringExceptionHandler() noexcept
		: exceptionHandlerHandle{ AddVectoredExceptionHandler( 1 /* FIRST */, exceptionHandler ) }
	{
	}
	~IgnoringVectoringExceptionHandler() noexcept
	{
		RemoveVectoredExceptionHandler( exceptionHandlerHandle );
	}
};

static std::string otel_procname()
{
	char process_exe[MAX_PATH]{};
	GetModuleFileNameA( nullptr, process_exe, DWORD( std::size( process_exe ) ) );
	auto p = std::max( strrchr( process_exe, '\\' ), strrchr( process_exe, '/' ) );
	p = p ? p + 1 : process_exe;
	_strlwr( p );
	return p;
}


static std::string otel_hostname()
{
	char hostName[1024]{};
	DWORD hostNameSize{ DWORD( std::size( hostName ) ) };
	GetComputerNameExA( ComputerNameDnsHostname, hostName, &hostNameSize );
	_strlwr( hostName );
	return hostName;
}


static std::string otel_username()
{
	char userName[1024]{};
	DWORD userNameSize{ DWORD( std::size( userName ) ) };
	GetUserNameA( userName, &userNameSize );
	if ( userName[0] == 0 )
	{
		strcpy_s( userName, "null_username" );
	}
	_strlwr( userName );
	return userName;
}


static std::vector<std::string> split_whitespace( const std::string_view &sv_ )
{
	std::vector<std::string> r;
	for ( auto sv{ sv_ }; !sv.empty(); )
	{
		// trim tabs & spaces
		while ( !sv.empty() && ( sv.front() == '\t' || sv.front() == ' ' ) )
			sv = sv.substr( 1 );
		// find first tab or space
		const auto tab_pos{ sv.find_first_of( '\t' ) };
		const auto space_pos{ sv.find_first_of( ' ' ) };
		const auto piece{ sv.substr( 0, std::min( space_pos, tab_pos ) ) };
		if ( piece.empty() )
		{
			break;
		}
		r.emplace_back( piece );
		sv = sv.substr( piece.size() );
	}
	return r;
}


static bool match_env_list( const char *envVar, const std::string &value )
{
	char buf[8192]{};
	GetEnvironmentVariableA( envVar, buf, sizeof( buf ) );
	const auto patterns{ split_whitespace( buf ) };
	return std::any_of( patterns.cbegin(), patterns.cend(), [&value]( const std::string &pattern )
	{
		return MatchWildcard( value.c_str(), pattern.c_str() );
	} );
}


static bool can_enable_otel_sdk()
{
	char buf[16]{};
	GetEnvironmentVariableA( "OTEL_ENABLED", buf, sizeof( buf ) );
	const bool enabled{ buf[0] == '1' && buf[1] == 0 };
	return enabled;
}


// Verify that otel_sdk_{r,d,rd}.dll can be used
// We do a similar check in otel_api_usable()
static bool otel_sdk_usable_noexhandler() noexcept
{
	bool enabled{ can_enable_otel_sdk() };
	if ( enabled )
	{
		__try
		{
			bool ignored{};
			// The function below is marked as __declspec(dllextern), hence calling it would ensure that the .dll is loaded
			// Calling it then, would make sure if it's usable if no exception is thrown.
			// We do a similar test in otel_api::otel_usable()
			opentelemetry::sdk::common::GetBoolEnvironmentVariable( "", ignored );
		}
		__except ( EXCEPTION_EXECUTE_HANDLER )
		{
			fprintf( stderr, "\nWARNING: [otel_sdk] Failed to use otel_sdk_{r,rd,d}.dll, error=0x%08.8X\n\n", GetExceptionCode() );
			enabled = false;
		}
	}
	// Update the variable for other systems, like `otel_api`
	SetEnvironmentVariableA( "OTEL_ENABLED", enabled ? "1" : nullptr );
	return enabled;
}


static bool otel_sdk_usable()
{
	// In case different vectored exception handler is install, ours would take priority
	otel_sdk::IgnoringVectoringExceptionHandler handler;
	return otel_sdk_usable_noexhandler();
}


// Create a low-cardinality (1..32, and occasionally above 32) value for the `process.vpid` attribute.
// The tuple (procname, vpid) is unique, much like pid is (e.g. GetCurrentProcessId()).
// The low cardinality prevents lots of timeseries from being created (for example in prometheus).
// NOTE: vpid=0 is never used.
static int otel_procvpid()
{
	std::random_device rand_dev;
	std::mt19937 rng{ rand_dev() };
	auto procname{ otel_procname() };
	for ( auto i = ( rng() & 0x1F ) + 1; i; i++ )
	{
		wchar_t event_name[MAX_PATH]{};
		swprintf_s( event_name, L"Global\\otelvpid:%S:%d", procname.c_str(), i );
		const HANDLE h{ CreateEventExW( nullptr, event_name, 0, 0 ) };
		if ( h )
		{
			return i;
		}
	}
	return 0;
}


class otel_sdk_log_handler : public opentelemetry::sdk::common::internal_log::DefaultLogHandler
{
	void Handle( opentelemetry::sdk::common::internal_log::LogLevel level, const char *file, int line, const char *msg, const opentelemetry::sdk::common::AttributeMap & ) noexcept override
	{
		const std::string fullMsg{ "  otel_sdk: [" + LevelToString( level ) + "] " + file + ":" + std::to_string( line ) + " - " + msg + "\n" };
		fputs( fullMsg.c_str(), stdout );
	}
};

static bool otel_sdk_enabled()
{
	static auto otel_usable{ otel_sdk_usable() };
	return otel_usable;
};


static void init_resource()
{
	if ( !otel_sdk_enabled() )
	{
		return;
	}

	using namespace sdk::resource;

	ResourceAttributes resources
	{
		{ SemanticConventions::kServiceName, otel_procname() },
		{ SemanticConventions::kHostName, otel_hostname() },
		{ SemanticConventions::kProcessVpid, otel_procvpid() },
	};

	auto resource = Resource::Create( resources );
	Resource::GetDefault() = resource;
}

struct Metrics
{
	const std::shared_ptr<metrics::MeterProvider> meterProvider{ metrics::Provider::GetMeterProvider() };
	const std::shared_ptr<metrics::Meter> meter{ meterProvider->GetMeter( "otel_sdk_metrics" ) };
	const std::shared_ptr<metrics::ObservableInstrument> process_uptime
	{
		meter->CreateDoubleObservableCounter( "process.uptime", "The time the process has been running" /* description */, "s" /* unit */ )
	};

	Metrics()
	{
		process_uptime->AddCallback( []( metrics::ObserverResult observer_result, void * )
		{
			int64_t value = clock();
			nostd::get<nostd::shared_ptr<metrics::ObserverResultT<double>>>( observer_result )->Observe( double( value ) / double( CLOCKS_PER_SEC ) );
		},
			nullptr );
	}

	static const Metrics &instance()
	{
		static const auto metrics = new Metrics;
		return *metrics;
	}
};

static const otel_sdk::Metrics *add_common_metrics()
{
	if ( !otel_sdk_enabled() )
	{
		return nullptr;
	}

	static const auto *metrics = &otel_sdk::Metrics::instance();
	return metrics;
}


static void init_metrics()
{
	if ( !otel_sdk_enabled() )
	{
		return;
	}

	exporter::otlp::OtlpGrpcMetricExporterOptions exporterOptions;
	// 43170 (e.g. unlike 4317) is our own otlp grpc proxy hosted in PipelineLogger ETW
	exporterOptions.endpoint = "127.0.0.1:43170";
	exporterOptions.compression = "none";
	exporterOptions.max_threads = 1;
	exporterOptions.max_concurrent_requests = 1024;
	auto exporter{ exporter::otlp::OtlpGrpcMetricExporterFactory::Create( exporterOptions ) };

	sdk::metrics::PeriodicExportingMetricReaderOptions readerOptions;
	readerOptions.export_interval_millis = std::chrono::milliseconds( 10000 );
	readerOptions.export_timeout_millis = std::chrono::milliseconds( 5000 );
	auto reader{ sdk::metrics::PeriodicExportingMetricReaderFactory::Create( std::move( exporter ), readerOptions ) };
	auto meterProvider{ sdk::metrics::MeterProviderFactory::Create() };
	meterProvider->AddMetricReader( std::move( reader ) );

	if ( extraDebug )
	{
		auto ostream_exporter = opentelemetry::exporter::metrics::OStreamMetricExporterFactory::Create();
		auto ostream_reader{ sdk::metrics::PeriodicExportingMetricReaderFactory::Create( std::move( ostream_exporter ), readerOptions ) };
		meterProvider->AddMetricReader( std::move( ostream_reader ) );
	}

	const std::unordered_set<std::string> dropUrlTemplateMap{ { opentelemetry::sdk::resource::SemanticConventions::kUrlTemplate } };
	auto dropUrlTemplateAttr{ std::make_unique<opentelemetry::sdk::metrics::DroppingAttributesProcessor>( dropUrlTemplateMap ) };

	// Remove any histogram buckets for now to reduce timeseries, we'll bring them back later on case by case basis.
	auto aggr_conf{ std::make_unique<sdk::metrics::HistogramAggregationConfig>() };
	aggr_conf->boundaries_.clear();
	aggr_conf->record_min_max_ = true;
	meterProvider->AddView(
		sdk::metrics::InstrumentSelectorFactory::Create( sdk::metrics::InstrumentType::kHistogram, "*", "" ),
		sdk::metrics::MeterSelectorFactory::Create( "", "", "" ),
		sdk::metrics::ViewFactory::Create(
			"",
			"",
			"",
			sdk::metrics::AggregationType::kHistogram,
			std::move( aggr_conf ),
			std::move( dropUrlTemplateAttr )
			)
		);

	metrics::Provider::SetMeterProvider( std::move( meterProvider ) );
}


static void init_logs()
{
	if ( !otel_sdk_enabled() )
	{
		return;
	}

	exporter::otlp::OtlpGrpcLogRecordExporterOptions exporterOptions;
	exporterOptions.endpoint = "127.0.0.1:43170";
	exporterOptions.compression = "none";
	exporterOptions.max_threads = 1;
	exporterOptions.max_concurrent_requests = 1024;
	auto exporter = exporter::otlp::OtlpGrpcLogRecordExporterFactory::Create( exporterOptions );
	sdk::logs::BatchLogRecordProcessorOptions processorOptions;
	processorOptions.max_export_batch_size = 8192;
	processorOptions.max_queue_size = 65536;
	processorOptions.schedule_delay_millis = std::chrono::milliseconds{ 10000 };
	auto processor = sdk::logs::BatchLogRecordProcessorFactory::Create( std::move( exporter ), processorOptions );

	std::vector<std::unique_ptr<sdk::logs::LogRecordProcessor>> logRecordProcessors;
	logRecordProcessors.emplace_back( std::move( processor ) );

	std::shared_ptr<logs::LoggerProvider> provider( sdk::logs::LoggerProviderFactory::Create( std::move( logRecordProcessors ) ) );
	logs::Provider::SetLoggerProvider( provider );
}


static void otel_shutdown()
{
	if ( !otel_sdk_enabled() )
	{
		return;
	}

	std::shared_ptr<metrics::MeterProvider> none;
	metrics::Provider::SetMeterProvider( none );
}


instance::instance()
{
	if ( !otel_sdk_enabled() )
	{
		return;
	}

	{
		using namespace sdk::common::internal_log;

		char logLevel[16]{};
		GetEnvironmentVariableA( "OTEL_LOG_LEVEL", logLevel, sizeof( logLevel ) );
		if ( logLevel[0] && logLevel[1] == 0 && ( logLevel[0] >= '1' && logLevel[0] <= '5' ) )
		{
			auto level = logLevel[0] - '0';
			extraDebug = level > 4;
			if ( level > 4 )
			{
				level = 4;
			}
			GlobalLogHandler::SetLogLevel( LogLevel( level ) );
			GlobalLogHandler::SetLogHandler( std::make_shared<otel_sdk_log_handler>() );
		}
		else
		{
			GlobalLogHandler::SetLogLevel( LogLevel::None );
			GlobalLogHandler::SetLogHandler( std::make_shared<NoopLogHandler>() );
		}
	}

	std::at_quick_exit( otel_shutdown );
	std::atexit( otel_shutdown );
	init_resource();
	init_metrics();
	init_logs();
	add_common_metrics();
}


instance::~instance()
{
	otel_shutdown();
}
} // namespace otel_sdk
