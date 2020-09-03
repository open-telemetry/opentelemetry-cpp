REM Change provider guid here:
set PROVIDER_GUID=49592B3E-B03E-5EBF-91E2-846A2E4904E5

del /Y Trace_000001.etl
logman stop MyTelemetryTraceData 
logman delete MyTelemetryTraceData
logman create trace MyTelemetryTraceData -p {%PROVIDER_GUID%} -o Trace.etl
logman start MyTelemetryTraceData
echo Capturing data for provider %PROVIDER_GUID% ...
pause
logman stop MyTelemetryTraceData 
