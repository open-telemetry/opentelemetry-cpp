$ErrorActionPreference = "Stop"
trap { $host.SetShouldExit(1) }

$BAZELISK_VERSION="1.7.4"
$CWD=(Get-Item -Path ".\").FullName
(new-object System.Net.WebClient). `
   DownloadFile("https://github.com/bazelbuild/bazelisk/releases/download/v$BAZELISK_VERSION/bazelisk-windows-amd64.exe",
                "C:\windows\system32\bazel.exe")
