set "PATH=%CD%\vcpkg;%PATH%"
call "%~dp0\vcvars.cmd"
call bootstrap-vcpkg.bat
vcpkg integrate install
vcpkg install gtest:x64-windows
vcpkg install benchmark:x64-windows
vcpkg install ms-gsl
