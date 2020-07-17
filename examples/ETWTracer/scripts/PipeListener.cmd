@echo off
set "PATH=C:\Windows\System32\WindowsPowerShell\v1.0\;%PATH%"
start powershell.exe -NoProfile -ExecutionPolicy Bypass -Command "& '.\PipeListener.ps1' %*"
