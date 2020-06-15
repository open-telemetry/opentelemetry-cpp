setlocal
pushd "%~dp0"
powershell -File .\run-tests-all.ps1 "%~dp0..\out"
popd
