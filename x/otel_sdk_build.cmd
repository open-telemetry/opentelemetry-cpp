@echo off
setlocal

for /F "usebackq delims=" %%i in (`where bazelisk bazel`) do set __BAZEL__=%%i
if "%__BAZEL__%"=="" goto:no-bazel

set PATH=

pushd "%~dp0"

if "%1"=="" goto:all
if "%1"=="test" goto:test
if "%1"=="zip" goto:zip
if "%1"=="shutdown" goto:shutdown

popd
endlocal
goto:eof

:all
call:test
call:zip
call:shutdown
goto:eof

:test
REM singleton_test does not work when linked as static under Windows
"%__BAZEL__%" test -k --profile=0.nodll.tracing.json --@otel_sdk//:with_dll=false -- @otel_sdk//... -@otel_sdk//api/test/singleton:singleton_test || echo a || goto:error
"%__BAZEL__%" build -k --profile=1.all.tracing.json --@otel_sdk//:with_dll=true -- @otel_sdk//... -@otel_sdk//:otel_sdk_zip ||  echo a || goto:error
"%__BAZEL__%" test -k --profile=2.dbg.tracing.json --@otel_sdk//:with_dll=true -c dbg -- @otel_sdk//... -@otel_sdk//:otel_sdk_zip ||  echo a || goto:error
"%__BAZEL__%" test -k --profile=3.fastbuild.tracing.json --@otel_sdk//:with_dll=true -c fastbuild -- @otel_sdk//... -@otel_sdk//:otel_sdk_zip || echo a ||  goto:error
"%__BAZEL__%" test -k --profile=4.opt.tracing.json --@otel_sdk//:with_dll=true -c opt -- @otel_sdk//... -@otel_sdk//:otel_sdk_zip ||  echo a || goto:error
goto:eof

:zip
"%__BAZEL__%" run -k --profile=5.pkg.tracing.json --@otel_sdk//:with_dll=true @otel_sdk//:make_otel_sdk ||  echo a || goto:error
goto:eof

:shutdown
"%__BAZEL__%" shutdown || goto:error
goto:eof

:no-bazel
echo FAILED: No bazelisk or bazel found!
exit 1
goto:eof

:error
echo FAILED!
exit 1
goto:eof
