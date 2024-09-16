$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

# Get vswhere and cmake
# ===================================
$env:VCPKG_ROOT = "$PWD/tools/vcpkg"
$env:VCPKG_CMAKE = "$PWD/tools/vcpkg/scripts/buildsystems/vcpkg.cmake"
$vswhere_dir = (Get-ChildItem -path ${env:ProgramFiles(x86)} -filter "vswhere.exe" -recurse -ErrorAction SilentlyContinue | Select-Object -First 1).DirectoryName
$cmake_dir = (Get-ChildItem -path ${env:ProgramFiles} -filter "cmake.exe" -recurse -ErrorAction SilentlyContinue | Select-Object -First 1).DirectoryName
$vcpkg_dir = (Get-ChildItem -path $env:VCPKG_ROOT -filter "vcpkg.exe" -recurse -ErrorAction SilentlyContinue | Select-Object -First 1).DirectoryName
if (-not $vswhere_dir) { throw "vswhere.exe not found" }
if (-not $cmake_dir) { throw "cmake.exe not found" }
if (-not $vcpkg_dir) { throw "vcpkg.exe not found" }
$env:PATH = "$vswhere_dir;$cmake_dir;$vcpkg_dir;${env:PATH}"

# Set Visual Studio environment variables from vcvarsall.bat
# ===================================
& $env:ComSpec /c "`"%VISUAL_STUDIO_PATH%\VC\Auxiliary\Build\vcvarsall.bat`" %OPENTELEMETRY_CPP_LIBARCH% & set" | ConvertFrom-String -Delimiter '=' | ForEach-Object {
    New-Item -Name $_.P1 -value $_.P2 -ItemType Variable -Path Env: -Force
}

# Build/Test/Package OpenTelemetry CPP
# ===================================
$install_dir = "$PWD/out"
$otel_build_options = @(
    # see CI "cmake.maintainer.sync.test"
    #"-DCMAKE_INSTALL_PREFIX=$install_dir"                      # Only for ninja builds
    "-DCMAKE_BUILD_TYPE=${env:OPENTELEMETRY_CPP_CONFIG}"        # Build only release
    #"-DWITH_STL=CXX17"                                          # Which version of the Standard Library for C++ to use, Matching bee_backend version
    "-DWITH_ABSEIL=ON"                                          # Don't use STL (mutually exclusive with WITH_STL), use Abseil instead
    "-DCMAKE_CXX_STANDARD=${env:CXX_STANDARD}"                  # Use C++ Standard Language Version 17, Matching bee_backend language version
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"                      # Add the -fPIC compiler option (off), as recommended by OpenTelemetry CPP documentation
    #"-DWITH_OTLP_GRPC=ON"                                       # Whether to include the OTLP gRPC exporter in the SDK (off), disabling it since it is apparently slow and requires additional dependencies
    "-DWITH_OTLP_HTTP=ON"                                       # Whether to include the OTLP http exporter in the SDK (off)
    "-DWITH_OTLP_HTTP_COMPRESSION=ON"                           # Whether to include gzip compression for the OTLP http exporter in the SDK (off)
    #"-DOPENTELEMETRY_INSTALL=ON"                               # ? Whether to install opentelemetry targets (on)
    #"-DWITH_ASYNC_EXPORT_PREVIEW=OFF"                          # ? Whether to enable async export (off)
    "-DOTELCPP_MAINTAINER_MODE=OFF"                             # Build in maintainer mode (-Wall -Werror), since -Wall is not well supported by Windows STL, I'm disabling it but would rather not
    "-DWITH_NO_DEPRECATED_CODE=ON"                              # Do not include deprecated code
    "-DWITH_DEPRECATED_SDK_FACTORY=OFF"                         # Don't compile deprecated SDK Factory
    #"-DWITH_ABI_VERSION_1=OFF"                                  # ABI version 1 (on)
    #"-DWITH_ABI_VERSION_2=ON"                                   # EXPERIMENTAL: ABI version 2 preview (off)
    "-DVCPKG_TARGET_TRIPLET=${env:OPENTELEMETRY_CPP_LIBTYPE}"   # Use static linked system dynamically linked libraries
    "-DCMAKE_TOOLCHAIN_FILE=${env:VCPKG_CMAKE}"                 # Use vcpkg toolchain file
    "-DBUILD_TESTING=OFF"                                       # Whether to enable tests (on), makes the build faster and it does not work with *-windows-static-md
    "-DWITH_EXAMPLES=OFF"                                       # Whether to build examples (on), makes the build faster and it does not work with *-windows-static-md
)
& cmake -S . -B build -A $env:OPENTELEMETRY_CPP_LIBARCH $otel_build_options
if ($LASTEXITCODE -ne 0) { throw "Failed to configure OpenTelemetry CPP" }
& cmake --build build --parallel $env:NUMBER_OF_PROCESSORS --config $env:OPENTELEMETRY_CPP_CONFIG --target all_build
if ($LASTEXITCODE -ne 0) { throw "Failed to build OpenTelemetry CPP" }
& ctest --build-config $env:OPENTELEMETRY_CPP_CONFIG --test-dir build
if ($LASTEXITCODE -ne 0) { throw "OpenTelemetry CPP Tests failed" }
& cmake --install build --prefix $install_dir --config $env:OPENTELEMETRY_CPP_CONFIG
if ($LASTEXITCODE -ne 0) { throw "Failed to install OpenTelemetry CPP" }
