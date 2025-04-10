#include "otel_api.h"

#include <windows.h>
#include <cstdio>

namespace otel_api
{
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

static bool otel_api_usable_noexhandler()
{
	char buf[16]{};
	// Gated feature as it's still experimental
	GetEnvironmentVariableA( "OTEL_ENABLED", buf, sizeof( buf ) );
	const bool OtelEnabled{ ( buf[0] == '1' && buf[1] == 0 ) };
	if ( !otelEnabled )
	{
		return false;
	}

	// Lookup if the dll has been loaded.
	auto mod{ GetModuleHandleW( L"otel_sdk_r" ) };
	if ( !mod )
	{
		mod = GetModuleHandleW( L"otel_sdk_d" );
	}
	if ( !mod )
	{
		mod = GetModuleHandleW( L"otel_sdk_rd" );
	}
	if ( !mod )
	{
		// No need to report anything here, it's expected that the dll might not be loaded.
		return false;
	}
	// Lookup a function we'll try to call. Similar test is done in otel_sdk::otel_usable
	const auto pGetBoolEnvironmentVariable{ reinterpret_cast<bool ( * )( const char *, bool &result )>( GetProcAddress( mod, "?GetBoolEnvironmentVariable@common@sdk@v2@opentelemetry@@YA_NPEBDAEA_N@Z" ) ) };
	if ( !pGetBoolEnvironmentVariable )
	{
		fprintf( stderr, "\nWARNING: [otel_api] Wrong otel_sdk_{r,rd,d}.dll loaded, function lookup failed\n\n" );
		return false;
	}
	DWORD exceptionCode{};
	__try
	{
		bool ignored{};
		// Try to call the function. If any exception happens, mark the loaded otel as unusable.
		pGetBoolEnvironmentVariable( "", ignored );
		return true;
	}
	__except ( EXCEPTION_EXECUTE_HANDLER )
	{
		exceptionCode = GetExceptionCode();
	}
	fprintf( stderr, "\nWARNING: [otel_api] Wrong otel_sdk_{r,rd,d}.dll loaded, error=0x%08.8X\n\n", exceptionCode );
	return false;
}


// Check if otel_sdk_{r,d,rd}.dll can be used
// We do a similar verification in otel_sdk_usable()
static bool otel_api_usable()
{
	// In case different vectored exception handler is install, ours would take priority
	otel_api::IgnoringVectoringExceptionHandler handler;
	return otel_api_usable_noexhandler();
}


bool enabled() noexcept
{
	static bool usable = otel_api_usable();
	// For now, if the API is usable, we treat it as being enabled too.
	return usable;
}
} // namespace otel_api
