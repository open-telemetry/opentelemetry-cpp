@echo off
pushd %~dp0
if "%~1"=="" goto help

WHERE choco >NUL 2>NUL
IF %ERRORLEVEL% NEQ 0 call tools\setup-choco.cmd

WHERE docker >NUL 2>NUL
IF "%ERRORLEVEL%"=="0" goto docker_ok
choco install -y docker-desktop
choco install -y docker-cli
:docker_ok

docker info
docker version

set IMAGE_NAME=%1
echo Running in container %IMAGE_NAME%
sc query com.docker.service

cd ..

REM Force reinstallation of build tools
del .buildtools 2>NUL
echo Building docker image...
docker build --rm -t %IMAGE_NAME% docker/%IMAGE_NAME%

echo Starting build...
docker run -it -v %CD%:/build %IMAGE_NAME% "/build/tools/build.sh" clean

popd
exit

:help
cd ..
echo.
echo Usage: build-docker.cmd [container_name]
echo.
echo Supported containers:
dir /B docker
popd
