# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

$ErrorActionPreference = "Stop"
trap { $host.SetShouldExit(1) }

$CMAKE_VERSION="3.16.3"
$CWD=(Get-Item -Path ".\").FullName
(new-object System.Net.WebClient). `
   DownloadFile("https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION-win64-x64.zip", `
                "$CWD\cmake-$CMAKE_VERSION-win64-x64.zip")

unzip cmake-$CMAKE_VERSION-win64-x64.zip

$ENV:PATH="$ENV:PATH;$CWD\cmake-$CMAKE_VERSION-win64-x64\bin"
cmake --help
[Environment]::SetEnvironmentVariable(
    "Path",
        [Environment]::GetEnvironmentVariable("Path", [EnvironmentVariableTarget]::Machine) + ";$CWD\cmake-$CMAKE_VERSION-win64-x64\bin",
            [EnvironmentVariableTarget]::Machine)
