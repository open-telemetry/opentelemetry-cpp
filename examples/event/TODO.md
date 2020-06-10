# TODO list

Explore the following options:

- https://github.com/fmtlib/fmt exporter

- Nicer custom codec support

- Support mime types to identify payload format sent to stream: application/x-msgpack, text/plain; charset=utf-8, application/json, etc.

- review WinRT API for ETW in-lieu of TraceLoggingDynamic.h in OSS.
  It's possible to use UWP API in console apps and services, but only on Windows 10 starting with 19041:
  https://docs.microsoft.com/en-us/uwp/api/windows.foundation.diagnostics.loggingfields?view=winrt-19041

# Supported Compilers

This code is known to work well with:
- Visual Studio 2019 on Windows 10
- AppleClang 11.0.3.11030032 on Mac OS X 10.15.5
- GCC 9.3.0 on Mac OS X 10.15.5
