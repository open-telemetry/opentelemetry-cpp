REM Change provider guid here:
set PROVIDER_GUID=b7aa4d18-240c-5f41-5852-817dbf477472

del /Y Trace_000001.etl
logman stop MyTelemetryTraceData 
logman delete MyTelemetryTraceData
logman create trace MyTelemetryTraceData -p {%PROVIDER_GUID%} -o Trace.etl
logman start MyTelemetryTraceData
pause
logman stop MyTelemetryTraceData 
