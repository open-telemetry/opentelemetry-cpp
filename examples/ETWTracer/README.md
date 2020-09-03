# Example of ETW Tracer

NOTE: `TraceLoggingDynamic.h` header is pending Microsoft CELA approval for OSS release (not currently included yet).

# Usage Instructions

Main application utilizes the following headers:
- **ETWProvider.hpp** - interface to ETW with dynamic manifest.
- **ETWTracer.hpp**   - header-only implementation of OpenTelemetry C++ SDK API surface that sends events to Event Tracing for Windows.

Step 1: Starting ETW Provider listener. **NOTE: must be run in elevated shell, e.g. Run as Administrator...**

Example script sequence of commands to start the listener `CaptureTrace-ISTraceLoggingProvider.cmd`

Start it in a separate privileged shell:

```
REM This Provider GUID is a hash of "ISTraceLoggingProvider" Provider Name.
set PROVIDER_GUID=49592B3E-B03E-5EBF-91E2-846A2E4904E5
REM Delete the old trace
del /Y Trace_000001.etl
REM Reset data collection
logman stop MyTelemetryTraceData 
logman delete MyTelemetryTraceData
REM Create data set
logman create trace MyTelemetryTraceData -p {%PROVIDER_GUID%} -o Trace.etl
REM Start collection
logman start MyTelemetryTraceData
echo Capturing data for provider %PROVIDER_GUID% ...
pause
REM Stop collection
logman stop MyTelemetryTraceData 
```

Step 2: Emitting structured ETW traces :

```
> ETWTracer.exe ISTraceLoggingProvider TestEvent

Testing span API...
Provider Name: ISTraceLoggingProvider
Provider GUID: 49592B3E-B03E-5EBF-91E2-846A2E4904E5
Event name:    TestEvent
StartSpan: MySpan
AddEvent: TestEvent
AddEvent: MyEvent2
AddEvent: MyEvent1DS
EndSpan
[ DONE ]
Testing ETW logging API...
Provider Name: ISTraceLoggingProvider
Provider GUID: 49592B3E-B03E-5EBF-91E2-846A2E4904E5
Event name:    TestEvent
Provider Handle: 0x1201f25f249690
etw.write...
etw.close...
[ DONE ]
```

See `main.cpp` for details.

Step 3: Go back to ETW Provider listener Window and press any key to stop it. Captured file `Trace_000001.etl` may be reviewed under:

  Control Panel >>
  Computer Management >>
  Performance >>
  Data Collector Sets >>
  User Defined >>
  MyTelemetryTraceData

# Deployment in production

Example requires contents of all `api/include/opentelemetry/` headers. There are no prebuilt binaries or libraries. Implementation is header-only.

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
