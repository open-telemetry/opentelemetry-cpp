<#
# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

.SYNOPSIS
	Install OpenTelemetry C++ third-party dependencies using CMake.

.DESCRIPTION
	PowerShell counterpart to ci/install_thirdparty.sh.

	This script configures and builds the CMake project in install/cmake to
	install third-party packages into a provided prefix.

.PARAMETER --install-dir
	Path where third-party packages will be installed (required).

.PARAMETER --tags-file
	File containing tags for third-party packages (optional).

.PARAMETER --packages
	Semicolon-separated list of packages to build (optional). Default installs all.

.EXAMPLE
	./ci/install_thirdparty.ps1 --install-dir C:/third_party

.EXAMPLE
	./ci/install_thirdparty.ps1 --install-dir C:/third_party --tags-file C:/tags.txt --packages "grpc;protobuf"
#>

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Show-Usage {
  $scriptName = Split-Path -Leaf $PSCommandPath
  Write-Host ('Usage: {0} --install-dir <path> [--tags-file <file>] [--packages "<pkg1>;<pkg2>"]' -f $scriptName)
  Write-Host "  --install-dir <path>             Path where third-party packages will be installed (required)"
  Write-Host "  --tags-file <file>               File containing tags for third-party packages (optional)"
  Write-Host '  --packages "<pkg1>;<pkg2>;..."   Semicolon-separated list of packages to build (optional). Default installs all third-party packages.'
  Write-Host '  --build-type <build type>        CMake build type (optional)'
  Write-Host '  --build-shared-libs <ON|OFF>     Build shared libraries (optional)'
  Write-Host "  -h, --help                       Show this help message"
}

function Invoke-External {
  param(
    [Parameter(Mandatory = $true)]
    [string] $FilePath,

    [Parameter(Mandatory = $false)]
    [string[]] $Arguments = @()
  )

  & $FilePath @Arguments
  $exitCode = $LASTEXITCODE
  if ($exitCode -ne 0) {
    throw "Command failed (exit $exitCode): $FilePath $($Arguments -join ' ')"
  }
}

$ThirdpartyTagsFile = ''
$ThirdpartyPackages = ''
$ThirdpartyInstallDir = ''
$CmakeBuildType = ''
$CmakeBuildSharedLibs = ''

# Match install_thirdparty.sh behavior: parse GNU-style args from $args.
for ($i = 0; $i -lt $args.Count; ) {
  $a = [string]$args[$i]

  switch ($a) {
    '--install-dir' {
      if (($i + 1) -ge $args.Count -or ([string]$args[$i + 1]).StartsWith('--')) {
        [Console]::Error.WriteLine('Error: --install-dir requires a value')
        Show-Usage
        exit 1
      }
      $ThirdpartyInstallDir = [string]$args[$i + 1]
      $i += 2
      continue
    }
    '--tags-file' {
      if (($i + 1) -ge $args.Count -or ([string]$args[$i + 1]).StartsWith('--')) {
        [Console]::Error.WriteLine('Error: --tags-file requires a value')
        Show-Usage
        exit 1
      }
      $ThirdpartyTagsFile = [string]$args[$i + 1]
      $i += 2
      continue
    }
    '--packages' {
      if (($i + 1) -ge $args.Count -or ([string]$args[$i + 1]).StartsWith('--')) {
        [Console]::Error.WriteLine('Error: --packages requires a value')
        Show-Usage
        exit 1
      }
      $ThirdpartyPackages = [string]$args[$i + 1]
      $i += 2
      continue
    }
    '--build-type' {
      if (($i + 1) -ge $args.Count -or ([string]$args[$i + 1]).StartsWith('--')) {
        [Console]::Error.WriteLine('Error: --build-type requires a value')
        Show-Usage
        exit 1
      }
      $CmakeBuildType = [string]$args[$i + 1]
      $i += 2
      continue
    }
    '--build-shared-libs' {
      if (($i + 1) -ge $args.Count -or ([string]$args[$i + 1]).StartsWith('--')) {
        [Console]::Error.WriteLine('Error: --build-shared-libs requires a value')
        Show-Usage
        exit 1
      }
      $CmakeBuildSharedLibs = [string]$args[$i + 1]
      $i += 2
      continue
    }
    '-h' { Show-Usage; exit 0 }
    '--help' { Show-Usage; exit 0 }
    default {
      [Console]::Error.WriteLine(("Unknown option: {0}" -f $a))
      Show-Usage
      exit 1
    }
  }
}

if ([string]::IsNullOrWhiteSpace($ThirdpartyInstallDir)) {
  [Console]::Error.WriteLine('Error: --install-dir is a required argument.')
  Show-Usage
  exit 1
}

$SrcDir = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Definition)

$CxxStandard = $env:CXX_STANDARD
if ([string]::IsNullOrWhiteSpace($CxxStandard)) {
  $CxxStandard = '17'
}

$TempRoot = $env:TEMP
if ([string]::IsNullOrWhiteSpace($TempRoot)) {
  # Fallback if TEMP isn't set.
  $TempRoot = [System.IO.Path]::GetTempPath()
}

$ThirdpartyBuildDir = Join-Path $TempRoot 'otel-cpp-third-party-build'
if (Test-Path -LiteralPath $ThirdpartyBuildDir) {
  Remove-Item -LiteralPath $ThirdpartyBuildDir -Recurse -Force
}

if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
  throw 'cmake was not found in PATH. Please install CMake and ensure it is available on PATH.'
}

$CmakeSourceDir = Join-Path $SrcDir 'install' 'cmake'
if (-not (Test-Path -LiteralPath $CmakeSourceDir)) {
  throw "CMake source directory not found: $CmakeSourceDir"
}

$OtelcppProtoPath = $env:OTELCPP_PROTO_PATH
if ($null -eq $OtelcppProtoPath) {
  $OtelcppProtoPath = ''
}

$CmakeConfigureArgs = @(
  '-S', $CmakeSourceDir,
  '-B', $ThirdpartyBuildDir,
  "-DCMAKE_INSTALL_PREFIX=$ThirdpartyInstallDir",
  "-DCMAKE_CXX_STANDARD=$CxxStandard",
  "-DOTELCPP_THIRDPARTY_TAGS_FILE=$ThirdpartyTagsFile",
  "-DOTELCPP_PROTO_PATH=$OtelcppProtoPath",
  "-DOTELCPP_THIRDPARTY_INSTALL_LIST=$ThirdpartyPackages"
)

$parallel = [Math]::Max([Environment]::ProcessorCount, 1)

$CmakeBuildArgs = @(
  '--build', $ThirdpartyBuildDir,
  '--clean-first',
  '--parallel', $parallel
)

if (-not [string]::IsNullOrWhiteSpace($CmakeBuildType)) {
  $CmakeConfigureArgs += "-DCMAKE_BUILD_TYPE=$CmakeBuildType"
  $CmakeBuildArgs += @('--config', $CmakeBuildType)
}

if (-not [string]::IsNullOrWhiteSpace($CmakeBuildSharedLibs)) {
  $CmakeConfigureArgs += "-DBUILD_SHARED_LIBS=$CmakeBuildSharedLibs"
}

Invoke-External -FilePath 'cmake' -Arguments $CmakeConfigureArgs

# Use CMake's cross-generator parallel flag.
Invoke-External -FilePath 'cmake' -Arguments $CmakeBuildArgs

# Keep parity with the bash script (no-op on Windows).
if ($ThirdpartyInstallDir -eq '/usr/local') {
  if (Get-Command ldconfig -ErrorAction SilentlyContinue) {
    Invoke-External -FilePath 'ldconfig' -Arguments @()
  }
}

Write-Host 'Third-party packages installed successfully.'
Write-Host "-- THIRDPARTY_INSTALL_DIR: $ThirdpartyInstallDir"
Write-Host "-- THIRDPARTY_TAGS_FILE: $ThirdpartyTagsFile"
if ([string]::IsNullOrWhiteSpace($ThirdpartyPackages)) {
  Write-Host '-- THIRDPARTY_PACKAGES: all'
}
else {
  Write-Host "-- THIRDPARTY_PACKAGES: $ThirdpartyPackages"
}
Write-Host "-- CXX_STANDARD: $CxxStandard"
if (-not [string]::IsNullOrWhiteSpace($CmakeBuildType)) {
  Write-Host "-- CMAKE_BUILD_TYPE: $CmakeBuildType"
}
