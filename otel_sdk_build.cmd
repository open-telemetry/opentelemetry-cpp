@echo off
setlocal
set BAZEL_SH=
set BAZEL_VC=
set BAZEL_VC_FULL_VERSION=

for /F "delims=" %%i in ("%ComSpec%") do set __COMSPEC_DIR__=%%~dpi
if "%__COMSPEC_DIR__%"=="" goto:no-comspec

for /F "usebackq delims=" %%i in (`where python`) do set __PYTHON_DIR__=%%~dpi
if "%__PYTHON_DIR__%"=="" goto:no-python

rem Look first for bazelisk, then bazel
for /F "usebackq delims=" %%i in (`where bazelisk bazel`) do set __BAZEL__=%%i
if "%__BAZEL__%"=="" goto:no-bazel

set PATH=%__COMSPEC_DIR__%;%__PYTHON_DIR__%
pushd "%~dp0"

REM singleton_test does not work when linked as static under Windows
"%__BAZEL__%" test --//:with_dll=false %* -- ... -//api/test/singleton:singleton_test || goto:error

REM Exclude building otel_sdk_zip right now, do it later. This warms up the tests bellow
"%__BAZEL__%" build %* --//:with_dll=true -- ... -otel_sdk_zip || goto:error

"%__BAZEL__%" test %* --//:with_dll=true -c dbg -- ... -otel_sdk_zip || goto:error
"%__BAZEL__%" test %* --//:with_dll=true -c fastbuild -- ... -otel_sdk_zip || goto:error
"%__BAZEL__%" test %* --//:with_dll=true -c opt -- ... -otel_sdk_zip || goto:error

"%__BAZEL__%" build %* --//:with_dll=true otel_sdk_zip || goto:error

for /F "usebackq delims=" %%i in (`"%__BAZEL__%" info execution_root 2^>nul`) do set __ROOT__=%%i
for /F "usebackq delims=" %%i in (`"%__BAZEL__%" cquery --//:with_dll^=true otel_sdk_zip --output^=files 2^>nul`) do set __ZIP__=%%i

if "%__ZIP__%"=="" goto:broken-build-zip-file

for %%i in ("%__ROOT__%/%__ZIP__%") do xcopy "%%~dpnxi" . /Y /F /V || goto:error

echo. ALL GOOD!
popd
endlocal
goto:eof

:no-bazel
echo FAILED: No bazelisk or bazel found!
exit /b 1
goto:eof

:no-python
echo FAILED: No python found!
exit /b 1
goto:eof

:no-comspec
echo FAILED: No ComSpec env var set!
exit /b 1
goto:eof

:error
echo FAILED!
exit /b 1
goto:eof

:broken-build-zip-file
echo The BUILD has broken otel_sdk.zip that can't be build for all compilation_modes
exit /b 1
goto:eof
