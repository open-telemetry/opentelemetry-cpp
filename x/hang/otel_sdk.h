#pragma once

// The `otel_sdk` library integrates for us the `OpenTelemetry c++
// It's meant to be used only in the application, as linking to it enforces loading of otel_sdk_{r,d,rd}.dll
//
// The `otel_api` is a helper library around OpenTelemetry C++'s `api` layer
// It's meant to be used anywhere instrumentation needs to be added
// Using it does not force loading of otel_sdk_{r,d,rd}.dll, or copying the .dll during build.
//


namespace otel_sdk
{
// Instantiate a single instance of the SDK in your main() function, like this:
//   otel_sdk::Instance otel_sdk_instance;
struct instance
{
	instance();
	~instance();
};
} // namespace otel_sdk
