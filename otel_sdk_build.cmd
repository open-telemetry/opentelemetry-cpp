@echo off
setlocal

for /F "delims=" %%i in ("%ComSpec%") do set __COMSPEC_DIR__=%%~dpi
if "%__COMSPEC_DIR__%"=="" goto:no-comspec

for /F "usebackq delims=" %%i in (`where python`) do set __PYTHON_DIR__=%%~dpi
if "%__PYTHON_DIR__%"=="" goto:no-python

rem Look first for bazelisk, then bazel
for /F "usebackq delims=" %%i in (`where bazelisk bazel`) do set __BAZEL__=%%i
if "%__BAZEL__%"=="" goto:no-bazel

set PATH=%__COMSPEC_DIR__%;%__PYTHON_DIR__%

set __COMSPEC_DIR__=
set __PYTHON_DIR__=

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
"%__BAZEL__%" test --profile=0.nodll.tracing.json --//:with_dll=false -- ... -//api/test/singleton:singleton_test || goto:error
"%__BAZEL__%" build --profile=1.all.tracing.json --//:with_dll=true -- ... -otel_sdk_zip || goto:error
"%__BAZEL__%" test --profile=2.dbg.tracing.json --//:with_dll=true -c dbg -- ... -otel_sdk_zip || goto:error
"%__BAZEL__%" test --profile=3.fastbuild.tracing.json --//:with_dll=true -c fastbuild -- ... -otel_sdk_zip || goto:error
"%__BAZEL__%" test --profile=4.opt.tracing.json --//:with_dll=true -c opt -- ... -otel_sdk_zip || goto:error
goto:eof

:zip
"%__BAZEL__%" run --profile=5.pkg.tracing.json --//:with_dll=true make_otel_sdk || goto:error
goto:eof

:shutdown
"%__BAZEL__%" shutdown || goto:error
goto:eof

:no-bazel
echo FAILED: No bazelisk or bazel found!
exit 1
goto:eof

:no-python
echo FAILED: No python found!
exit 1
goto:eof

:no-comspec
echo FAILED: No ComSpec env var set!
exit 1
goto:eof

:error
echo FAILED!
exit 1
goto:eof
