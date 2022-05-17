$ErrorActionPreference = "Stop";
trap { $host.SetShouldExit(1) }

$action = $args[0]

$SRC_DIR = (Get-Item -Path ".\").FullName

$BAZEL_OPTIONS = "--copt=-DENABLE_METRICS_PREVIEW --copt=-DENABLE_LOGS_PREVIEW"
$BAZEL_TEST_OPTIONS = "$BAZEL_OPTIONS --test_output=errors"

if (!(test-path build)) {
  mkdir build
}
$BUILD_DIR = Join-Path "$SRC_DIR" "build"

if (!(test-path plugin)) {
  mkdir plugin
}
$PLUGIN_DIR = Join-Path "$SRC_DIR" "plugin"

$VCPKG_DIR = Join-Path "$SRC_DIR" "tools" "vcpkg"

switch ($action) {
  "bazel.build" {
    bazel build --copt=-DENABLE_TEST $BAZEL_OPTIONS --action_env=VCPKG_DIR=$VCPKG_DIR -- //...
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
  }
  "cmake.test" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      -DVCPKG_TARGET_TRIPLET=x64-windows `
      "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cmake --build .
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    ctest -C Debug
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
  }
  "cmake.exporter.otprotocol.test" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      -DVCPKG_TARGET_TRIPLET=x64-windows `
      -DWITH_OTPROTCOL=ON `
      "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cmake --build .
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    ctest -C Debug
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
  }
  "cmake.build_example_plugin" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      -DVCPKG_TARGET_TRIPLET=x64-windows `
      "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cmake --build .
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cp examples/plugin/plugin/Debug/example_plugin.dll ${PLUGIN_DIR}
  }
  "cmake.test_example_plugin" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      -DVCPKG_TARGET_TRIPLET=x64-windows `
      "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cmake --build .
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cp examples/plugin/plugin/Debug/example_plugin.dll ${PLUGIN_DIR}
    $config = New-TemporaryFile
    examples/plugin/load/Debug/load_plugin_example.exe ${PLUGIN_DIR}/example_plugin.dll $config
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
  }
  default {
    echo "unknown action: $action"
    exit 1
  }
}
