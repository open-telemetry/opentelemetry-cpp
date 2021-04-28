pushd "%~dp0"
set "PATH=%CD%;%PATH%"
cd ..
if not exist "build" (
  mkdir build
)
if not exist "plugin" (
  mkdir plugin
)
bazel.exe build %* -- //... //api/test/... //sdk/test/...
popd
