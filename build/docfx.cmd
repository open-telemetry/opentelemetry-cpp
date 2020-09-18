SETLOCAL
SETLOCAL ENABLEEXTENSIONS

docfx build docs\docfx.json > docfx.log
DEL docs\docfx.json 2> NUL
DEL docs\toc.yml 2> NUL
@IF NOT %ERRORLEVEL% == 0 (
  type docfx.log
  ECHO Error: docfx build failed. 1>&2
  EXIT /B %ERRORLEVEL%
)
@type docfx.log
@type docfx.log | findstr /C:"Build succeeded."
@IF NOT %ERRORLEVEL% == 0 (
  ECHO Error: you have introduced build warnings. 1>&2
  EXIT /B %ERRORLEVEL%
)
