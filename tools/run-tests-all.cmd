@echo on
set ROOT=%~dp0\..
cd %~dp0

REM Run tests for nostd containers implementation
set OUTDIR=%ROOT%\out.nostd
call run-tests.cmd %OUTDIR% > %OUTDIR%\test_results.log

REM Run tests for STL containers implementation
set OUTDIR=%ROOT%\out.stl
call run-tests.cmd %OUTDIR% > %OUTDIR%\test_results.log
