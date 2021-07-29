@echo off
pushd %~dp0
set "PATH=%ProgramFiles%\LLVM\bin;%PATH%"
set BUILDTOOLS_VERSION=clang
set CMAKE_GEN=Ninja
call build.cmd %*
popd
