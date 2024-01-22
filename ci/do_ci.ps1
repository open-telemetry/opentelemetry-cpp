# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

$ErrorActionPreference = "Stop";
trap { $host.SetShouldExit(1) }

$action = $args[0]

$nproc = (Get-ComputerInfo).CsNumberOfLogicalProcessors

$SRC_DIR = (Get-Item -Path ".\").FullName

$BAZEL_OPTIONS = "--copt=-DENABLE_ASYNC_EXPORT"
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
    bazel build $BAZEL_OPTIONS --action_env=VCPKG_DIR=$VCPKG_DIR --deleted_packages=opentracing-shim -- //...
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
    cmake --build . -j $nproc
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
  "cmake.dll.test" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      -DVCPKG_TARGET_TRIPLET=x64-windows `
      -DOPENTELEMETRY_BUILD_DLL=1 `
     "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cmake --build . -j $nproc
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    ctest -C Debug
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    $env:PATH = "$BUILD_DIR\ext\src\dll\Debug;$env:PATH"
    examples\simple\Debug\example_simple.exe
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    examples\metrics_simple\Debug\metrics_ostream_example.exe
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    examples\logs_simple\Debug\example_logs_simple.exe
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
  }
  "cmake.dll.cxx20.test" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      -DCMAKE_CXX_STANDARD=20 `
      -DVCPKG_TARGET_TRIPLET=x64-windows `
      -DOPENTELEMETRY_BUILD_DLL=1 `
     "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cmake --build . -j $nproc
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    ctest -C Debug
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    $env:PATH = "$BUILD_DIR\ext\src\dll\Debug;$env:PATH"
    examples\simple\Debug\example_simple.exe
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    examples\metrics_simple\Debug\metrics_ostream_example.exe
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    examples\logs_simple\Debug\example_logs_simple.exe
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
  }
  "cmake.maintainer.test" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      -DOTELCPP_MAINTAINER_MODE=ON `
      -DWITH_NO_DEPRECATED_CODE=ON `
      -DVCPKG_TARGET_TRIPLET=x64-windows `
      "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cmake --build . -j $nproc
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
  "cmake.maintainer.cxx20.stl.test" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      -DWITH_STL=CXX20 `
      -DCMAKE_CXX_STANDARD=20 `
      -DOTELCPP_MAINTAINER_MODE=ON `
      -DWITH_NO_DEPRECATED_CODE=ON `
      -DVCPKG_TARGET_TRIPLET=x64-windows `
      "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cmake --build . -j $nproc
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
  "cmake.with_async_export.test" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      -DVCPKG_TARGET_TRIPLET=x64-windows `
      -DWITH_ASYNC_EXPORT_PREVIEW=ON `
      "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cmake --build . -j $nproc
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
    cmake --build . -j $nproc
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
  "cmake.exporter.otprotocol.dll.test" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      -DVCPKG_TARGET_TRIPLET=x64-windows `
      -DOPENTELEMETRY_BUILD_DLL=1 `
      -DWITH_OTPROTCOL=ON `
      "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cmake --build . -j $nproc
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
  "cmake.exporter.otprotocol.with_async_export.test" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      -DVCPKG_TARGET_TRIPLET=x64-windows `
      -DWITH_ASYNC_EXPORT_PREVIEW=ON `
      -DWITH_OTPROTCOL=ON `
      "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cmake --build . -j $nproc
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
      -DWITH_ASYNC_EXPORT_PREVIEW=ON `
      "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cmake --build . -j $nproc
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
      -DWITH_ASYNC_EXPORT_PREVIEW=ON `
      "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
    cmake --build . -j $nproc
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
