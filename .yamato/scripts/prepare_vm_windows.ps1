$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

# Install CMake
# ===================================
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    sudo choco install -y cmake
    if ($LASTEXITCODE -ne 0) { throw "Could not install cmake" }
}

# Get vswhere and cmake
# ===================================
$vswhere_dir = (Get-ChildItem -path ${env:ProgramFiles(x86)} -filter "vswhere.exe" -recurse -ErrorAction SilentlyContinue | Select-Object -First 1).DirectoryName
$cmake_dir = (Get-ChildItem -path ${env:ProgramFiles} -filter "cmake.exe" -recurse -ErrorAction SilentlyContinue | Select-Object -First 1).DirectoryName
if (-not $vswhere_dir) { throw "vswhere.exe not found" }
if (-not $cmake_dir) { throw "cmake.exe not found" }
$env:PATH = "$vswhere_dir;$cmake_dir;${env:PATH}"

# Copy python-3.11.5-embed-amd64.zip since Unity firewall prevents downloads from www.python.org
# ===================================
#$downloads = mkdir "$PWD/tools/vcpkg/downloads" -force
#Copy-Item "$PWD/.yamato/bin/python-3.11.5-embed-amd64.zip" "$downloads/python-3.11.5-embed-amd64.zip" -verbose -force

# Fix wrong version of build tools leading to "error LNK2001: unresolved external symbol _Thrd_sleep_for"
# ===================================
# sudo { & "${env:ProgramFiles(x86)}/Microsoft Visual Studio/Installer/setup.exe" update --installPath $env:VISUAL_STUDIO_PATH --quiet --norestart | out-default }

# Set Visual Studio environment variables from vcvarsall.bat
# ===================================
& $env:ComSpec /c "`"%VISUAL_STUDIO_PATH%\VC\Auxiliary\Build\vcvarsall.bat`" %OPENTELEMETRY_CPP_LIBARCH% & set" | ConvertFrom-String -Delimiter '=' | ForEach-Object {
    New-Item -Name $_.P1 -value $_.P2 -ItemType Variable -Path Env: -Force
}

# Install vcpkg
# ===================================
$env:VCPKG_ROOT = "$PWD/tools/vcpkg"
$env:VCPKG_CMAKE = "$PWD/tools/vcpkg/scripts/buildsystems/vcpkg.cmake"
push-location $env:VCPKG_ROOT
& $PWD/scripts/bootstrap.ps1 -disableMetrics
$vcpkg_dir = (Get-ChildItem -path $env:VCPKG_ROOT -filter "vcpkg.exe" -recurse -ErrorAction SilentlyContinue | Select-Object -First 1).DirectoryName
if (-not $vcpkg_dir) { throw "vcpkg.exe not found" }
$env:PATH = "$vcpkg_dir;$env:PATH"
& vcpkg integrate install
if ($LASTEXITCODE -ne 0) { throw "Failed to integrate vcpkg" }
Pop-Location # $env:VCPKG_ROOT

# Install vcpkg dependencies
# ===================================
$vcpkg_dependencies = @(
    "gtest:${env:OPENTELEMETRY_CPP_LIBTYPE}",
    "benchmark:${env:OPENTELEMETRY_CPP_LIBTYPE}",
    "protobuf:${env:OPENTELEMETRY_CPP_LIBTYPE}",
    #"ms-gsl:${env:OPENTELEMETRY_CPP_LIBTYPE}",
    "nlohmann-json:${env:OPENTELEMETRY_CPP_LIBTYPE}",
    "abseil:${env:OPENTELEMETRY_CPP_LIBTYPE}",
    #"gRPC:${env:OPENTELEMETRY_CPP_LIBTYPE}",
    #"prometheus-cpp:${env:OPENTELEMETRY_CPP_LIBTYPE}",
    "curl:${env:OPENTELEMETRY_CPP_LIBTYPE}"
    #"thrift:${env:OPENTELEMETRY_CPP_LIBTYPE}",
)
foreach ($dep in $vcpkg_dependencies) {
    if ($dep -like "protobuf:*" -or $dep -like "benchmark:*") {
        & vcpkg "--vcpkg-root=${env:VCPKG_ROOT}" install --overlay-ports=`"${env:VCPKG_ROOT}/ports`" $dep
    }
    else {
        & vcpkg "--vcpkg-root=${env:VCPKG_ROOT}" install $dep
    }
    if ($LASTEXITCODE -ne 0) { throw "Failed to install $dep" }
}
