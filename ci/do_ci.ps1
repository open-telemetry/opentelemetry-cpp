# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

$ErrorActionPreference = "Stop";
trap { $host.SetShouldExit(1) }

$action = $args[0]

$nproc = (Get-ComputerInfo).CsNumberOfLogicalProcessors

$SRC_DIR = (Get-Item -Path ".\").FullName

# Workaround https://github.com/bazelbuild/bazel/issues/18683
$BAZEL_STARTUP_OPTIONS = "--output_base=C:\O"
$BAZEL_OPTIONS = "--copt=-DENABLE_ASYNC_EXPORT --compilation_mode=dbg"
$BAZEL_TEST_OPTIONS = "$BAZEL_OPTIONS --test_output=errors"

if (!(test-path build)) {
  mkdir build
}
$BUILD_DIR = Join-Path "$SRC_DIR" "build"

if (!(test-path install_test)) {
  mkdir install_test
}
$INSTALL_TEST_DIR = Join-Path "$SRC_DIR" "install_test"

if (!(test-path plugin)) {
  mkdir plugin
}
$PLUGIN_DIR = Join-Path "$SRC_DIR" "plugin"

$VCPKG_DIR = Join-Path "$SRC_DIR" "tools/vcpkg"

$Env:CTEST_OUTPUT_ON_FAILURE = "1"

function FindAndMergeDllPath {
  param (
    [string[]]$AdditionalDirs
  )
  
  $FINAL_PATH = $env:PATH
  $PATH_SET = @{}
  $PATH_ITEMS = $FINAL_PATH -split [IO.Path]::PathSeparator
  foreach ($item in $PATH_ITEMS) {
    $PATH_SET[$item] = $true
  }

  foreach ($dir in $AdditionalDirs) {
    $GLOB_PATTERN = Join-Path "$dir" "*.dll"
    $DETECTED_DLL_FILES = Get-ChildItem -Path $GLOB_PATTERN -Recurse
    $DETECTED_DLL_DIRS = $(foreach ($dll_file in $DETECTED_DLL_FILES) {
        $dll_file.Directory.FullName
      }) | Sort-Object | Get-Unique
    foreach ($dll_dir in $DETECTED_DLL_DIRS) {
      if (-not $PATH_SET.ContainsKey($dll_dir)) {
        $FINAL_PATH = "$dll_dir" + [IO.Path]::PathSeparator + $FINAL_PATH
        $PATH_SET[$dll_dir] = $true
      }
    }
  }

  return $FINAL_PATH
}

switch ($action) {
  "bazel.build" {
    bazel $BAZEL_STARTUP_OPTIONS build $BAZEL_OPTIONS --action_env=VCPKG_DIR=$VCPKG_DIR --deleted_packages=opentracing-shim -- //...
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
      -DOTELCPP_BUILD_DLL=1 `
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
    $env:PATH = FindAndMergeDllPath "$BUILD_DIR\ext\src\dll\Debug"
    ctest -C Debug
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
      -DOTELCPP_BUILD_DLL=1 `
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
    $env:PATH = FindAndMergeDllPath "$BUILD_DIR\ext\src\dll\Debug"
    ctest -C Debug
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
  }
  "cmake.maintainer.test" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      "-C $SRC_DIR/test_common/cmake/all-options-abiv1-preview.cmake" `
      -DOTELCPP_WITH_OPENTRACING=OFF `
      -DOTELCPP_MAINTAINER_MODE=ON `
      -DOTELCPP_WITH_NO_DEPRECATED_CODE=ON `
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

    $env:PATH = FindAndMergeDllPath "."
    Write-Output "PATH=$env:PATH"

    ctest -C Debug
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
  }
  "cmake.maintainer.cxx20.stl.test" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      "-C $SRC_DIR/test_common/cmake/all-options-abiv1-preview.cmake" `
      -DOTELCPP_WITH_OPENTRACING=OFF `
      -DOTELCPP_WITH_STL=CXX20 `
      -DCMAKE_CXX_STANDARD=20 `
      -DOTELCPP_MAINTAINER_MODE=ON `
      -DOTELCPP_WITH_NO_DEPRECATED_CODE=ON `
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

    $env:PATH = FindAndMergeDllPath "."
    Write-Output "PATH=$env:PATH"

    ctest -C Debug
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
  }
  "cmake.maintainer.abiv2.test" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      "-C $SRC_DIR/test_common/cmake/all-options-abiv2-preview.cmake" `
      -DOTELCPP_WITH_OPENTRACING=OFF `
      -DOTELCPP_MAINTAINER_MODE=ON `
      -DOTELCPP_WITH_NO_DEPRECATED_CODE=ON `
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

    $env:PATH = FindAndMergeDllPath "."
    Write-Output "PATH=$env:PATH"

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
      -DOTELCPP_WITH_ASYNC_EXPORT_PREVIEW=ON `
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

    $env:PATH = FindAndMergeDllPath "."
    Write-Output "PATH=$env:PATH"

    ctest -C Debug
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
  }
  "cmake.exporter.otprotocol.test" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      "-C $SRC_DIR/test_common/cmake/all-options-abiv1-preview.cmake" `
      -DOTELCPP_WITH_OPENTRACING=OFF `
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

    $env:PATH = FindAndMergeDllPath "."
    Write-Output "PATH=$env:PATH"

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
      -DOTELCPP_BUILD_DLL=1 `
      -DOTELCPP_WITH_OTLP_GRPC=ON `
      -DOTELCPP_WITH_OTLP_HTTP=ON `
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
    $env:PATH = FindAndMergeDllPath "$BUILD_DIR\ext\src\dll\Debug"
    ctest -C Debug
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }
  }
  "cmake.exporter.otprotocol.with_async_export.test" {
    cd "$BUILD_DIR"
    cmake $SRC_DIR `
      "-C $SRC_DIR/test_common/cmake/all-options-abiv1-preview.cmake" `
      -DOTELCPP_WITH_OPENTRACING=OFF `
      -DVCPKG_TARGET_TRIPLET=x64-windows `
      -DOTELCPP_WITH_ASYNC_EXPORT_PREVIEW=ON `
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

    $env:PATH = FindAndMergeDllPath "."
    Write-Output "PATH=$env:PATH"

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
      -DOTELCPP_WITH_ASYNC_EXPORT_PREVIEW=ON `
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
      -DOTELCPP_WITH_ASYNC_EXPORT_PREVIEW=ON `
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
  "cmake.install.test" {
    Remove-Item -Recurse -Force "$BUILD_DIR\*"
    Remove-Item -Recurse -Force "$INSTALL_TEST_DIR\*"
    cd "$BUILD_DIR"

    if (Test-Path Env:\CXX_STANDARD) {
      $CXX_STANDARD = [int](Get-Item Env:\CXX_STANDARD).Value
    }
    else {
      $CXX_STANDARD = 17
    }
    if (-not $CXX_STANDARD) {
      $CXX_STANDARD = 17
    }
    Write-Host "Using CXX_STANDARD: $CXX_STANDARD"
      
    $CMAKE_OPTIONS = @(
      "-DCMAKE_CXX_STANDARD=$CXX_STANDARD",
      "-DCMAKE_CXX_STANDARD_REQUIRED=ON",
      "-DCMAKE_CXX_EXTENSIONS=OFF",
      "-DVCPKG_TARGET_TRIPLET=x64-windows",
      "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake"
    )

    cmake $SRC_DIR `
      $CMAKE_OPTIONS `
      "-DCMAKE_INSTALL_PREFIX=$INSTALL_TEST_DIR" `
      "-C $SRC_DIR/test_common/cmake/all-options-abiv2-preview.cmake" `
      -DOTELCPP_WITH_OPENTRACING=OFF `
      -DOTELCPP_WITH_GSL=ON `
      -DOTELCPP_INSTALL=ON

    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }

    cmake --build . -j $nproc
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }

    $env:PATH = FindAndMergeDllPath "."

    Write-Output "PATH=$env:PATH"

    ctest -C Debug
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }

    cmake --build . --target install
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }

    $env:PATH = FindAndMergeDllPath "$INSTALL_TEST_DIR\bin"

    Write-Output "PATH=$env:PATH"

    $CMAKE_OPTIONS_STRING = $CMAKE_OPTIONS -join " "
   
    $EXPECTED_COMPONENTS = @(
      "api",
      "sdk",
      "ext_common",
      "ext_http_curl",
      "exporters_in_memory",
      "exporters_ostream",
      "exporters_otlp_common",
      "exporters_otlp_file",
      "exporters_otlp_grpc",
      "exporters_otlp_http",
      "exporters_prometheus",
      "exporters_elasticsearch",
      "exporters_zipkin",
      "exporters_etw",
      "resource_detectors"
    )
    $EXPECTED_COMPONENTS_STRING = $EXPECTED_COMPONENTS -join ";"

    mkdir "$BUILD_DIR\install_test"
    cd "$BUILD_DIR\install_test"

    $env:PATH = FindAndMergeDllPath "."
    Write-Output "PATH=$env:PATH"

    cmake $CMAKE_OPTIONS `
      "-DCMAKE_PREFIX_PATH=$INSTALL_TEST_DIR" `
      "-DINSTALL_TEST_CMAKE_OPTIONS=$CMAKE_OPTIONS_STRING" `
      "-DINSTALL_TEST_COMPONENTS=$EXPECTED_COMPONENTS_STRING" `
      -S "$SRC_DIR\install\test\cmake"
          
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }

    ctest -C Debug --output-on-failure
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }

    exit 0
  }
  "cmake.dll.install.test" {
    cd "$BUILD_DIR"
    Remove-Item -Recurse -Force "$BUILD_DIR\*"
    Remove-Item -Recurse -Force "$INSTALL_TEST_DIR\*"

    $CMAKE_OPTIONS = @(
      "-DCMAKE_CXX_STANDARD=17",
      "-DVCPKG_TARGET_TRIPLET=x64-windows",
      "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake",
      "-DOTELCPP_BUILD_DLL=1"
    )

    cmake $SRC_DIR `
      $CMAKE_OPTIONS `
      "-DCMAKE_INSTALL_PREFIX=$INSTALL_TEST_DIR" `
      "-C $SRC_DIR/test_common/cmake/all-options-abiv1-preview.cmake" `
      -DOTELCPP_INSTALL=ON `
      -DOTELCPP_WITH_CONFIGURATION=OFF `
      -DOTELCPP_WITH_OPENTRACING=OFF `
      -DOTELCPP_WITH_OTLP_GRPC_SSL_MTLS_PREVIEW=OFF `
      -DOTELCPP_WITH_OTLP_GRPC_CREDENTIAL_PREVIEW=OFF `
      -DOTELCPP_WITH_OTLP_RETRY_PREVIEW=OFF `
      -DOTELCPP_WITH_OTLP_GRPC=OFF `
      -DOTELCPP_WITH_OTLP_HTTP=OFF `
      -DOTELCPP_WITH_OTLP_FILE=OFF `
      -DOTELCPP_WITH_OTLP_HTTP_COMPRESSION=OFF `
      -DOTELCPP_WITH_HTTP_CLIENT_CURL=OFF `
      -DOTELCPP_WITH_PROMETHEUS=OFF `
      -DOTELCPP_WITH_ZIPKIN=OFF `
      -DOTELCPP_WITH_ELASTICSEARCH=OFF `
      -DOTELCPP_WITH_EXAMPLES=OFF `
      -DOTELCPP_WITH_EXAMPLES_HTTP=OFF

    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }

    cmake --build . -j $nproc
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }

    $env:PATH = FindAndMergeDllPath "."

    Write-Output "PATH=$env:PATH"

    ctest -C Debug

    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }

    cmake --build . --target install
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }

    $env:PATH = FindAndMergeDllPath "$INSTALL_TEST_DIR\bin"

    Write-Output "PATH=$env:PATH"

    $CMAKE_OPTIONS_STRING = $CMAKE_OPTIONS -join " "

    $EXPECTED_COMPONENTS = @(
      "api",
      "sdk",
      "ext_common",
      "exporters_in_memory",
      "exporters_ostream",
      "exporters_etw",
      "ext_dll" 
    )
    $EXPECTED_COMPONENTS_STRING = $EXPECTED_COMPONENTS -join ";"

    mkdir "$BUILD_DIR\install_test"
    cd "$BUILD_DIR\install_test"

    $env:PATH = FindAndMergeDllPath "."

    Write-Output "PATH=$env:PATH"

    cmake $CMAKE_OPTIONS `
      "-DCMAKE_PREFIX_PATH=$INSTALL_TEST_DIR" `
      "-DINSTALL_TEST_CMAKE_OPTIONS=$CMAKE_OPTIONS_STRING" `
      "-DINSTALL_TEST_COMPONENTS=$EXPECTED_COMPONENTS_STRING" `
      -S "$SRC_DIR\install\test\cmake"
          
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }

    ctest -C Debug --output-on-failure
    $exit = $LASTEXITCODE
    if ($exit -ne 0) {
      exit $exit
    }

    exit 0
  }
  default {
    echo "unknown action: $action"
    exit 1
  }
}
