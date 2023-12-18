@echo off
setlocal
rem I've had these setup on my machine, these should not be present, but wanted to have bit cleaner build
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

"%__BAZEL__%" build -k --//:with_dll=true ...

rem Note that this builds (through the magic of force_debug/release/reldeb) all configurations (unlike tests).
rem Note that `otel_sdk.zip` is built here for the default fastdbg (e.g. when no -c is specified)
"%__BAZEL__%" build --//:with_dll=true ... || goto:error

rem We can't test dbg, fastbuild and opt at the same time, as done above ^^^ (no config "transition" possible when doing testing (AFAIK))
"%__BAZEL__%" test -k --//:with_dll=true --test_size_filters=small,medium,large,enormous --test_timeout_filters=short,moderate,long,eternal --test_verbose_timeout_warnings -c dbg ... || goto:error
"%__BAZEL__%" test -k --//:with_dll=true --test_size_filters=small,medium,large,enormous --test_timeout_filters=short,moderate,long,eternal --test_verbose_timeout_warnings -c fastbuild ... || goto:error
"%__BAZEL__%" test -k --//:with_dll=true --test_size_filters=small,medium,large,enormous --test_timeout_filters=short,moderate,long,eternal --test_verbose_timeout_warnings -c opt ... || goto:error

for /F "usebackq" %%i in (`"%__BAZEL__%" info execution_root 2^>nul`) do set __ROOT__=%%i
for /F "usebackq" %%i in (`"%__BAZEL__%" cquery --//:with_dll^=true otel_sdk_zip --output^=files 2^>nul`) do set __ZIP__=%%i

if "%__ZIP__%"=="" goto:broken-build-zip-file

for %%i in ("%__ROOT__%/%__ZIP__%") do xcopy "%%~dpnxi" . /D /Y || goto:error

echo. ALL GOOD!
popd
endlocal
goto:eof

:no-bazel
echo FAILED: No bazelisk or bazel found!
goto:eof

:no-python
echo FAILED: No python found!
goto:eof

:no-comspec
echo FAILED: No ComSpec env var set!
goto:eof

:error
echo FAILED!
goto:eof

:broken-build-zip-file
echo The BUILD has broken otel_sdk.zip that can't be build for all compilation_modes
goto:eof
