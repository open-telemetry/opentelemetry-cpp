# otel_sdk integratione example

The `x.cpp` has been copied from the `opentelemetry-cpp` examples folder (the "simple" examples), into single file.

In our version it links to the dynamic `opentelemetry-cpp` from the parent repo.

In the future I'll add static linking too, through the `dll_deps` macro.

# Notes with Aspire.Dashboard

- follow https://learn.microsoft.com/en-us/dotnet/aspire/fundamentals/dashboard/configuration?tabs=bash
- just set ASPNETCORE_URLS=http://localhost:18888
- and DOTNET_DASHBOARD_OTLP_ENDPOINT_URL=http://localhost:18889
bazel test --test_env=OTEL_EXPORTER_OTLP_ENDPOINT=http://127.0.0.1:18889 x
