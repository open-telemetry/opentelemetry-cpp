@echo off
setlocal

for /F "usebackq delims=" %%i in (`where bazelisk1.exe bazel1.exe`) do set __BAZEL__=%%i
if "%__BAZEL__%"=="" IF EXIST "%LOCALAPPDATA%\Microsoft\WinGet\Links\bazelisk.exe" set __BAZEL__=%LOCALAPPDATA%\Microsoft\WinGet\Links\bazelisk.exe
if "%__BAZEL__%"=="" IF EXIST "%LOCALAPPDATA%\Microsoft\WinGet\Links\bazel.exe" set __BAZEL__=%LOCALAPPDATA%\Microsoft\WinGet\Links\bazel.exe
if "%__BAZEL__%"=="" goto:no-bazel

set PATH=

pushd "%~dp0"

echo BAZEL="%__BAZEL__%"
"%__BAZEL__%" version

if "%1"=="" goto:all
if "%1"=="test" goto:test
if "%1"=="zip" goto:zip
if "%1"=="minimal" goto:minimal
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
"%__BAZEL__%" test  --profile=0.nodllk.tracing.json -k --//:with_dll=false -- ... -//api/test/singleton:singleton_test
"%__BAZEL__%" build --profile=1.allk.tracing.json -k --//:with_dll=true -- ... -otel_sdk_zip
"%__BAZEL__%" test  --profile=2.dbgk.tracing.json -k --//:with_dll=true -c dbg -- ... -otel_sdk_zip
"%__BAZEL__%" test  --profile=3.fastbuildk.tracing.json -k --//:with_dll=true -c fastbuild -- ... -otel_sdk_zip
"%__BAZEL__%" test  --profile=4.optk.tracing.json -k --//:with_dll=true -c opt -- ... -otel_sdk_zip
"%__BAZEL__%" run --profile=5.pkgk.tracing.json -k --//:with_dll=true make_otel_sdk

"%__BAZEL__%" test  --profile=0.nodll.tracing.json --//:with_dll=false -- ... -//api/test/singleton:singleton_test || goto:error
"%__BAZEL__%" build --profile=1.all.tracing.json --//:with_dll=true -- ... -otel_sdk_zip || goto:error
"%__BAZEL__%" test  --profile=2.dbg.tracing.json --//:with_dll=true -c dbg -- ... -otel_sdk_zip || goto:error
"%__BAZEL__%" test  --profile=3.fastbuild.tracing.json --//:with_dll=true -c fastbuild -- ... -otel_sdk_zip || goto:error
"%__BAZEL__%" test  --profile=4.opt.tracing.json --//:with_dll=true -c opt -- ... -otel_sdk_zip || goto:error
goto:eof

:minimal
"%__BAZEL__%" build --profile=min.nodll.tracing.json --//:with_dll=false api_sdk_includes || goto:error
"%__BAZEL__%" build --profile=min.dll.tracing.json --//:with_dll=true otel_sdk_files || goto:error
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

:error
echo FAILED!
exit 1
goto:eof
