endlocal
set "PATH=%~dp0;%PATH%"
set "TOOLS_PATH=%~dp0"
set "TOOLS_PATH_UNIX=%TOOLS_PATH:\=/%"
git config alias.cl !%TOOLS_PATH_UNIX%git-cl.cmd
exit /b 0
