@echo off
pushd %~dp0
set "PATH=%ProgramFiles%\LLVM-12\bin;%PATH%"
set BUILDTOOLS_VERSION=clang-12
set CMAKE_GEN=Ninja
call build.cmd %*
popd
