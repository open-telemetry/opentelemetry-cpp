#!/bin/sh
if [ "format" = "$1" ]; then
  if [ -z "$2" ]; then
    echo Please specify file name.
    exit
  fi

  if [ -f "$2" ]; then
    FILEPATH=$2
  else
    FILEPATH=${GIT_PREFIX}$2
  fi
  echo Formatting $FILEPATH
  clang-format -style=file -i $FILEPATH
fi
