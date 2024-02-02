
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include <opentelemetry/version.h>
#include <opentelemetry/metrics/provider.h>
#include <opentelemetry/trace/provider.h>
#include <opentelemetry/logs/provider.h>
#include <opentelemetry/sdk/common/env_variables.h>
#include <opentelemetry/sdk/common/global_log_handler.h>
#include <opentelemetry/sdk/resource/semantic_conventions.h>
#include <opentelemetry/sdk/metrics/export/periodic_exporting_metric_reader_factory.h>
#include <opentelemetry/sdk/metrics/meter_provider_factory.h>
#include <opentelemetry/sdk/metrics/meter_provider.h>
#include <opentelemetry/sdk/trace/batch_span_processor_factory.h>
#include <opentelemetry/sdk/trace/batch_span_processor_options.h>
#include <opentelemetry/sdk/trace/tracer_provider_factory.h>
#include <opentelemetry/sdk/trace/tracer_provider.h>
#include <opentelemetry/sdk/trace/processor.h>
#include <opentelemetry/sdk/logs/batch_log_record_processor_factory.h>
#include <opentelemetry/sdk/logs/batch_log_record_processor_options.h>
#include <opentelemetry/sdk/logs/logger_provider_factory.h>
#include <opentelemetry/sdk/logs/processor.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_metric_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_exporter_factory.h>
#include <opentelemetry/exporters/otlp/otlp_grpc_log_record_exporter_factory.h>
#include <opentelemetry/ext/http/client/http_client_factory.h>

#include <process.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <functional>
#include <iostream>
#include <iostream>
#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <string>
#include <thread>
#include <vector>

struct Process {
	std::string name;
	std::string bin;
	std::vector<std::string> args;
	std::thread runner;
	std::thread waiter;
	std::string healthCheckURL;
	std::string startURL;
	intptr_t handle{};
	intptr_t exitHandle{};
	int exitCode{};
	volatile bool waitFinished{};
	volatile bool healthy{};
	volatile bool exited{};
};

static const char* cmd_exe = std::getenv("ComSpec");

void waiter(Process* proc)
{
	proc->exitHandle = _cwait(&proc->exitCode, proc->handle, 0);
	proc->waitFinished = true;
}

void runner(Process* proc)
{
	std::string cmd;
	for( const auto& arg : proc->args )
	{
		if( !cmd.empty() )
			cmd.append( " " );
		cmd.append( arg );
	}
	printf("%s: Running %s %s\n", proc->name.c_str(), proc->bin.c_str(), cmd.c_str() );
	proc->handle = _spawnl( _P_NOWAIT, cmd_exe ? cmd_exe : "c:\\windows\\system32\\cmd.exe", "/c", "start", ("\"" + proc->name + "\"").c_str(), "/MIN", "/WAIT", proc->bin.c_str(), cmd.c_str(), nullptr );
	if( proc->handle == -1 )
	{
		proc->exited = true;
		printf("%s: %s failed!\n", proc->name.c_str(), proc->bin.c_str() );	
		return;
	}
	proc->waiter = std::thread(waiter, proc);
	int state{-1};
	while( !proc->waitFinished )
	{
		if( !proc->healthCheckURL.empty() )
		{
			const auto client{ opentelemetry::ext::http::client::HttpClientFactory::CreateSync() };
			const auto r{ client->GetNoSsl( proc->healthCheckURL ) };
			if( r )
			{
				proc->healthy = true;
				if( state != 1 )
					printf("%s: healthy\n", proc->name.c_str());
					state = 1;
			}
			else
			{
				proc->healthy = false;
				if( state != 0 )
					printf("%s: not healthy\n", proc->name.c_str());
				if( state == 1 )
					break;
				state = 0;
			}
		}
 		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	if( proc->waiter.joinable() )
		proc->waiter.join();
	proc->exited = true;
	printf("%s: %s exited with %d (exitHandle=%zd)\n", proc->name.c_str(), proc->bin.c_str(), proc->exitCode, proc->exitHandle);
}

void demo();

int main(int argc, const char*argv[])
{
	std::vector<std::string> procNames{ "loki", "tempo", "otel", "prom", "graf" };
	std::map<std::string, Process> procs;

	const auto cwd_path{ std::filesystem::current_path() };
	printf("cwd=%s\n", cwd_path.u8string().c_str() );

	for(int i=1; i<argc; i++ )
	{
		const std::string arg{ argv[i] };
		std::string key, val;
		const auto eqpos{ arg.find_first_of('=') };
		if( eqpos != std::string::npos )
		{
			key = arg.substr(0, eqpos);
			val = arg.substr(eqpos+1);
		}
		if( key.empty() || val.empty() )
		{
			printf("Invalid argument: %s\n", arg.c_str());
			return 1;
		}
//		printf("key=%s val=%s\n", key.c_str(), val.c_str());
		for( const auto& procName : procNames )
		{
			auto it = procs.try_emplace(procName, Process{procName});
			auto& proc = it.first->second;
			if( key == "--" + procName + "-bin" )
				proc.bin = val;
			else if( key == "--" + procName + "-arg" )
				proc.args.push_back(val);
			else if( key == "--" + procName + "-chk" )
				proc.healthCheckURL = val;
			else if( key == "--" + procName + "-url" )
				proc.startURL = val;
		}
	}

	for( auto it = procs.begin(); it != procs.end(); )
	{
		if( it->second.bin.empty() )
			it = procs.erase(it);
		else
			++it;
	}

	for( auto& it : procs )
	{
		auto& proc = it.second;
		proc.runner = std::thread( runner, &proc );
	}

	bool allHealthyOnce = true;
	for( ;; ) 
	{
		bool allHealthy = true;
		// for( const auto& it : procs )
		// {
		// 	const auto& proc = it.second;
		// 	if( proc.exited )
		// 		return 1;
		// 	//printf("%s=%d\n\n", proc.bin.c_str(), proc.healthy );
		// 	if( !proc.healthy )
		// 		allHealthy = false;
		// }
		if( allHealthy )
		{
			if( allHealthyOnce )
			{
				printf("ALL HEALTHY\n");
				for( const auto& it : procs )
				{
					const auto& proc = it.second;
					if( !proc.startURL.empty() )
					{
						system(("start \"\" " + proc.startURL).c_str());
					}
				}
				demo();
			}
			allHealthyOnce = false;
		}
 		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	return 0;
}

// ---------------------------

static opentelemetry::trace::Tracer *init_tracer()
{
	const auto tracer{ opentelemetry::trace::Provider::GetTracerProvider()->GetTracer( "otelx_tests", OPENTELEMETRY_SDK_VERSION ).get() };
#if OPENTELEMETRY_ABI_VERSION_NO >= 2	
	return tracer ? tracer : opentelemetry::trace::NoopTracerProvider().GetTracer( "otelx_tests", "", "", {} ).get();
#else
	return tracer ? tracer : opentelemetry::trace::NoopTracerProvider().GetTracer( "otelx_tests", "", "" ).get();
#endif
}

class MyLogger : public opentelemetry::logs::Logger
{
public:
	void SetMinimumSeverity( uint8_t severity_or_max ) noexcept
	{
		Logger::SetMinimumSeverity( severity_or_max );
	}
};

static opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> init_logger()
{
	auto logger{ opentelemetry::logs::Provider::GetLoggerProvider()->GetLogger( "otelx_tests", OPENTELEMETRY_SDK_VERSION ) };
	if ( const auto sdkLogger = logger.get() )
	{
		if ( const auto apiLogger = dynamic_cast<opentelemetry::logs::Logger *>( sdkLogger ) )
		{
			if ( const auto myLogger = static_cast<MyLogger *>( apiLogger ) )
			{
				//myLogger->SetMinimumSeverity(0);
			}
		}
		return logger;
	}
	return opentelemetry::logs::NoopLoggerProvider().GetLogger( "otelx_tests", "", "", "", opentelemetry::common::NoopKeyValueIterable() );
}


opentelemetry::trace::Tracer *get_tracer()
{
	static auto s_tracer{ init_tracer() };
	return s_tracer;
}


opentelemetry::logs::Logger *get_logger()
{
	static auto s_logger{ init_logger() };
	return s_logger.get();
}


static std::map<std::string, std::string> get_random_attr()
{
	const std::vector<std::pair<std::string, std::string>> labels
	{
		{ "key1", "value1" },
		{ "key2", "value2" },
		{ "key3", "value3" },
		{ "key4", "value4" },
		{ "key5", "value5" }
	};
	return { labels[rand() % ( labels.size() - 1 )], labels[rand() % ( labels.size() - 1 )] };
}

static void counter_example( const std::string &name, double startValue, const opentelemetry::trace::SpanContext &spanContext )
{
	get_logger()->Info( "counter_example" );
	opentelemetry::trace::StartSpanOptions options;
	options.parent = spanContext;
	auto span{ get_tracer()->StartSpan( "counter_example", options ) };

	const auto provider{ opentelemetry::metrics::Provider::GetMeterProvider() };
	const auto meter{ provider->GetMeter( name ) };
	const auto double_counter{ meter->CreateDoubleCounter( name + "_counter" ) };
	const auto double_updown_counter{ meter->CreateDoubleUpDownCounter( name + "_updown_counter" ) };

	double_counter->Add( startValue );
	double_updown_counter->Add( startValue + 500.0 );

	for ( auto i = 0; i < 1000; ++i )
	{
		double_counter->Add( 1.0 );
		double_updown_counter->Add( 1.0 );
		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	}
}


static void measurement_fetcher( opentelemetry::metrics::ObserverResult observer_result, void * /* state */ )
{
	get_logger()->Info( "measurement_fetcher" );
	static double value{ 0.0 };

	if ( const auto sharedObserver{ opentelemetry::nostd::get_if<opentelemetry::nostd::shared_ptr<opentelemetry::metrics::ObserverResultT<double>>>( &observer_result ) } )
	{
		if ( const auto observer{ sharedObserver->get() } )
		{
			const double random_incr{ rand() % 5 + 1.1 };

			value += random_incr;

			const auto labels{ get_random_attr() };
			const auto labelkv{ opentelemetry::common::KeyValueIterableView<decltype( labels )>{ labels } };

			observer->Observe( value, labelkv );
		}
	}
}



static void observable_counter_example( const std::string &name, const opentelemetry::trace::SpanContext &spanContext )
{
	get_logger()->Info( "observable_counter_example" );
	opentelemetry::trace::StartSpanOptions options;
	options.parent = spanContext;
	const auto span{ get_tracer()->StartSpan( "observable_counter_example", options ) };

	const auto provider{ opentelemetry::metrics::Provider::GetMeterProvider() };
	const auto meter{ provider->GetMeter( name ) };

	const auto double_observable_counter{ meter->CreateDoubleObservableCounter( name + "_observable_counter" ) };
	double_observable_counter->AddCallback( measurement_fetcher, nullptr );

	for ( auto i = 0; i < 10; ++i )
	{
		get_logger()->Info( "observable_counter_example sleeping for a second", i );
		std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
	}

	double_observable_counter->RemoveCallback( measurement_fetcher, nullptr );
}


static void histogram_example( const std::string &name, const opentelemetry::trace::SpanContext &spanContext )
{
	get_logger()->Info( "histogram_example" );
	opentelemetry::trace::StartSpanOptions options;
	options.parent = spanContext;
	const auto span{ get_tracer()->StartSpan( "histogram_example", options ) };

	const auto provider{ opentelemetry::metrics::Provider::GetMeterProvider() };
	const auto meter{ provider->GetMeter( name ) };

	const auto histogram_counter{ meter->CreateDoubleHistogram( name + "_histogram", "des", "unit" ) };
	const auto context{ opentelemetry::context::Context{} };

	for ( auto i = 0; i < 1000; ++i )
	{
		const double val{ rand() % 7890 + 0.5 };

		const auto labels{ get_random_attr() };
		const auto labelkv{ opentelemetry::common::KeyValueIterableView<decltype( labels )>{ labels } };

		histogram_counter->Record( val, labelkv, context );

		std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
	}
}

struct ResetMeterProvider
{
	~ResetMeterProvider()
	{
		get_logger()->Info( "~ResetMeterProvider()" );
		std::shared_ptr<opentelemetry::metrics::MeterProvider> none;
		opentelemetry::metrics::Provider::SetMeterProvider( none );
	}
};

class OpenTelemetryLogHandler : public opentelemetry::sdk::common::internal_log::DefaultLogHandler
{
	std::mutex m_alreadySeenMessagesLock;
	std::unordered_set<std::string> m_alreadySeenMessages;
	void Handle( opentelemetry::sdk::common::internal_log::LogLevel level, const char *file, int line, const char *msg, const opentelemetry::sdk::common::AttributeMap &attributes ) noexcept override
	{
		const std::string fullMsg{ "OpenTelemetry " + LevelToString( level ) + ": " + file + ":" + std::to_string( line ) + " - " + msg + "\n" };
		OutputDebugStringA( fullMsg.c_str() );
		{
			// Avoid spam to stdout/stderr which the DefaultLogHandler would print out to, but it'll keep all messages to OutputDebugString.
			std::lock_guard log( m_alreadySeenMessagesLock );
			if ( !m_alreadySeenMessages.emplace( std::move( fullMsg ) ).second )
			{
				return;
			}
		}
		DefaultLogHandler::Handle( level, file, line, msg, attributes );
	}
};

void setup()
{
	opentelemetry::sdk::common::internal_log::GlobalLogHandler::SetLogHandler( opentelemetry::nostd::shared_ptr<OpenTelemetryLogHandler>( new OpenTelemetryLogHandler() ) );
	char hostName[1024]{};
	DWORD hostNameSize = sizeof( hostName );
	GetComputerNameExA( ComputerNameDnsFullyQualified, hostName, &hostNameSize );
	auto resource = opentelemetry::sdk::resource::Resource::Create(
		{
			{ opentelemetry::sdk::resource::SemanticConventions::kServiceName, "otelx_demo" },
			{ opentelemetry::sdk::resource::SemanticConventions::kHostName, hostName },
			// { opentelemetry::sdk::resource::SemanticConventions::kProcessCommand, argv[0] },
			// { opentelemetry::sdk::resource::SemanticConventions::kProcessCommandLine, GetCommandLineA() },
		}
		);

	opentelemetry::sdk::resource::Resource::GetDefault() = resource;
}

void logger_setup()
{
			using namespace opentelemetry;
			{

				exporter::otlp::OtlpGrpcExporterOptions exporterOptions;

				// GTEST_LOG_( INFO ) << "\n"
				// 				   << "otlp trace exporter: " << exporterOptions.endpoint << "\n"
				// 				   << "otlp trace user_agent: " << exporterOptions.user_agent << "\n";
				
				auto exporter{ exporter::otlp::OtlpGrpcExporterFactory::Create( exporterOptions ) };

				sdk::trace::BatchSpanProcessorOptions batchProcessorOptions;
				batchProcessorOptions.max_export_batch_size = 4096;
				batchProcessorOptions.max_queue_size = 16384;
				batchProcessorOptions.schedule_delay_millis = std::chrono::milliseconds{ 1000 };
				auto batchProcessor{ sdk::trace::BatchSpanProcessorFactory::Create( std::move( exporter ), batchProcessorOptions ) };

				std::vector<std::unique_ptr<sdk::trace::SpanProcessor>> spanProcessors;
				spanProcessors.emplace_back( std::move( batchProcessor ) );
				const std::shared_ptr tracerProvider{ sdk::trace::TracerProviderFactory::Create( std::move( spanProcessors ) ) };

				trace::Provider::SetTracerProvider( tracerProvider );
			}

			{
				exporter::otlp::OtlpGrpcLogRecordExporterOptions log_opts;
				auto exporter = exporter::otlp::OtlpGrpcLogRecordExporterFactory::Create( log_opts );
				sdk::logs::BatchLogRecordProcessorOptions processorOptions;
				processorOptions.max_export_batch_size = 4096;
				processorOptions.max_queue_size = 16384;
				processorOptions.schedule_delay_millis = std::chrono::milliseconds{ 1000 };
				auto processor = sdk::logs::BatchLogRecordProcessorFactory::Create( std::move( exporter ), processorOptions );

				std::vector<std::unique_ptr<sdk::logs::LogRecordProcessor>> logRecordProcessors;
				logRecordProcessors.emplace_back( std::move( processor ) );

#if 0
				exporter::logs::ElasticsearchExporterOptions elasticOptions;
				elasticOptions.console_debug_ = true;
				elasticOptions.port_ = 9200;
				elasticOptions.host_ = "localhost:";
				elasticOptions.index_ = "otel2";
				{
					nostd::unique_ptr<sdk::logs::LogRecordExporter> elasticExporter( new exporter::logs::ElasticsearchLogRecordExporter(elasticOptions) );
					sdk::logs::BatchLogRecordProcessorOptions processorOptions2;
					processorOptions2.max_export_batch_size = 128;
					processorOptions2.max_queue_size = 256;
					processorOptions2.schedule_delay_millis = std::chrono::milliseconds{ 1000 };
					auto elasticProcessor = sdk::logs::BatchLogRecordProcessorFactory::Create(std::move(elasticExporter), processorOptions);
					logRecordProcessors.emplace_back(std::move(elasticProcessor));
				}
#endif // #if 0

				nostd::shared_ptr<logs::LoggerProvider> provider( sdk::logs::LoggerProviderFactory::Create( std::move( logRecordProcessors ) ) );
				logs::Provider::SetLoggerProvider( provider );
			}

}

void ui_main();

void demo()
{
	setup();
	logger_setup();

	opentelemetry::trace::StartSpanOptions options;
	options.parent = opentelemetry::context::RuntimeContext::GetCurrent();
	const auto span{ get_tracer()->StartSpan( "otelx_metrics_span", options ) };

	using namespace opentelemetry;

	ResetMeterProvider resetMeterProvider;

	//TODO: Redirect open telemetry's internal log to gtest log.

	const std::string metric_prefix_name{ "otelx_metrics_test" };

	const exporter::otlp::OtlpGrpcMetricExporterOptions exporterOptions;
	// GTEST_LOG_( INFO ) << "metric prefix: " << exporterOptions.endpoint << "\n"
	// 				   << "otel endpoint: " << exporterOptions.endpoint << "\n"
	// 				   << "otel user_agent: " << exporterOptions.user_agent << "\n"
	// 				   << "promxy data url: "
	// 				   << "http://promxy/graph?g0.expr=%7Bservice_name%3D%22otelx_%22%7D&g0.tab=0&g0.stacked=0&g0.show_exemplars=0&g0.range_input=12h"
	// 				   << "\n";

	auto exporter{ exporter::otlp::OtlpGrpcMetricExporterFactory::Create( exporterOptions ) };

	sdk::metrics::PeriodicExportingMetricReaderOptions readerOptions;
	readerOptions.export_interval_millis = std::chrono::milliseconds( 200 );
	readerOptions.export_timeout_millis = std::chrono::milliseconds( 100 );
	auto reader{ sdk::metrics::PeriodicExportingMetricReaderFactory::Create( std::move( exporter ), readerOptions ) };

	auto meterProvider{ sdk::metrics::MeterProviderFactory::Create() };
	{
		auto meterProviderPtr{ dynamic_cast<opentelemetry::sdk::metrics::MeterProvider *>( meterProvider.get() ) };
		//ASSERT_NE( nullptr, meterProviderPtr );
		meterProviderPtr->AddMetricReader( std::move( reader ) );
	}

	{
		std::shared_ptr provider( std::move( meterProvider ) );
		metrics::Provider::SetMeterProvider( provider );
		//ASSERT_EQ( provider, metrics::Provider::GetMeterProvider() );
	}

	get_logger()->Info( "Spawning threads" );

	auto context = span->GetContext();
	std::thread counter_example_thread1( counter_example, metric_prefix_name + "_0", 100.0, context );
	std::thread counter_example_thread2( counter_example, metric_prefix_name + "_1", 200.0, context );
	std::thread counter_example_thread3( counter_example, metric_prefix_name + "_1", 200.0, context );
	std::thread counter_example_thread4( counter_example, metric_prefix_name + "_1", 300.0, context );
	//std::thread observable_counter_example_thread( observable_counter_example, metric_prefix_name, context );
	//std::thread histogram_example_thread( histogram_example, metric_prefix_name, context );

	get_logger()->Info( "Joining threads" );

	//ui_main();

	counter_example_thread1.join();
	counter_example_thread2.join();
	counter_example_thread3.join();
	counter_example_thread4.join();
	//observable_counter_example_thread.join();
	//histogram_example_thread.join();

	get_logger()->Info( "Joined all threads" );
}

#include <windows.h>
#include <cstdlib>

void BindStdHandlesToConsole()
{
#ifdef _WIN32
	FreeConsole();
	AllocConsole();
#endif

    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stderr);
    freopen("CONOUT$", "w", stdout);
    
#ifdef _WIN32
    // Note that there is no CONERR$ file
    HANDLE hStdout = CreateFileA("CONOUT$",  GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE hStdin = CreateFileA("CONIN$",  GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    SetStdHandle(STD_OUTPUT_HANDLE,hStdout);
    SetStdHandle(STD_ERROR_HANDLE,hStdout);
    SetStdHandle(STD_INPUT_HANDLE,hStdin);
#endif

    // //Clear the error state for each of the C++ standard stream objects. 
    std::wclog.clear();
    std::clog.clear();
    std::wcout.clear();
    std::wcerr.clear();
    std::wcin.clear();
    std::cerr.clear();
    std::cout.clear();
    std::cin.clear();
}

void ui_main() {
  BindStdHandlesToConsole();
  using namespace ftxui;
  auto screen = ScreenInteractive::Fullscreen(); //TerminalOutput();

  std::vector<std::string> entries = {
      "entry 1",
      "entry 2",
      "entry 3",
  };
  int selected = 0;

  MenuOption option;
//  option.on_enter = screen.ExitLoopClosure();
  auto menu = Menu(&entries, &selected, option);

  screen.Loop(menu);

  std::cout << "Selected element = " << selected << std::endl;
}
