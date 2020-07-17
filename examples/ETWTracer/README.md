# Example of ETW Tracer

NOTE: `TraceLoggingDynamic.h` header is pending CELA approval for OSS-approved and is not currently included in this repo.

# Usage Instructions

Main application utilizes the following headers:
- **ETWProvider.hpp** - interface to ETW with dynamic manifest.
- **ETWTracer.hpp**   - header-only implementation of OpenTelemetry C++ SDK API surface that sends events to Event Tracing for Windows.

See `main.cpp` for details.

# Deployment instructions

Example requires contents of `api/include/opentelemetry/` headers. There are no prebuilt binaries or libraries:

- Open Visual Studio solution `msbuild/opentelemetry-cpp.sln`
- Open `ETWTracer` project
- Review the code, modify destination Provider GUID
- Build `ETWTracer` project
- Launch it to emit ETW events to your ETW Provider GUID

# Mapping ProviderName to GUID

See `scripts/StringToGUID.cmd` for the algorithm used to generate GUID based on Provider Name.

# Using SilkETW ETW listener for local event flow debugging

SilkETW & SilkService are flexible C# wrappers for ETW, they are meant to abstract away the complexities of ETW and give people
a simple interface to perform research and introspection. This fork https://github.com/maxgolov/SilkETW contains changes needed
to support:
- ETW manifested events and better support for ETW dynamic manifest events.
- ETW-XML events.
- Forwarding output from SilkETW listener to another process using named pipe.

Running `SilkETW` tool as Administrator / Elevated:

```
SilkETW.exe -l verbose -t user -pn MyProviderName -ot file -p .\output.json
```
where `MyProviderName`- ETW provider Name or GUID the tool is listening to. This must match the parameter passed to GetTracer(providerName) API.

## Using SilkETW to transform ETW to JSON


In order to listen to Provider GUID `{6d084bbf-6a96-44ef-83F4-0a77c9e34580}` and forward output stream as JSON over pipe for realtime inspection, launch SilkETW as follows:

```
SilkETW.exe -l verbose -t user -pn 6d084bbf-6a96-44ef-83F4-0a77c9e34580 -ot file -p \\.\pipe\ETW\6d084bbf-6a96-44ef-83F4-0a77c9e34580
```

to redirect the output to pipe. Corresponding PowerShell script for listening to ETW events coming to pipe **PipeListener.ps1** :

```
while (1)
{
  $pipe=new-object System.IO.Pipes.NamedPipeServerStream("ETW-6d084bbf-6a96-44ef-83F4-0a77c9e34580");
  $pipe.WaitForConnection();
  $sr = new-object System.IO.StreamReader($pipe);
  try
  {
    while ($cmd= $sr.ReadLine()) 
    {
      $cmd
    };
  } catch [System.IO.IOException]
  {
  }
  $sr.Dispose();
  $pipe.Dispose();
}
```

Pipe listener displays all incoming events in realtime.
