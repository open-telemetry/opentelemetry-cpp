$ErrorActionPreference = "Stop";
trap { $host.SetShouldExit(1) }

$action = $args[0]

$SRC_DIR=(Get-Item -Path ".\").FullName
mkdir build
$BUILD_DIR="$SRC_DIR\build"
$PLUGIN_DIR="$SRC_DIR\plugin"
mkdir plugin

$VCPKG_DIR="$SRC_DIR\vcpkg"

switch ($action) {
  "cmake.test" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
          -DVCPKG_TARGET_TRIPLET=x64-windows `
          "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR\scripts\buildsystems\vcpkg.cmake"
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
          "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR\scripts\buildsystems\vcpkg.cmake"
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cmake --build .
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cp examples\plugin\plugin\Debug\example_plugin.dll ${PLUGIN_DIR}
  }
  "cmake.test_example_plugin" {
    cd "$BUILD_DIR"
    rm -rf *
    cmake $SRC_DIR `
          -DVCPKG_TARGET_TRIPLET=x64-windows `
          "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR\scripts\buildsystems\vcpkg.cmake"
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    make load_plugin_example
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    examples/plugin/load/Debug/load_plugin_example.exe ${PLUGIN_DIR}/example_plugin.dll $null
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
