#!/bin/bash
if [[ ! -e tools/format.sh ]]; then
  echo "This tool must be run from the topmost directory." >&2
  exit 1
fi

set -e

FIND="find . -name .git -prune -o -name _deps -prune -o -name .build -prune -o"

# GNU syntax.
SED=(sed -i)
if [[ "$(uname)" = "Darwin" ]]; then
  SED=(sed -i "")
fi

# Correct common miscapitalizations.
"${SED[@]}" 's/Open[t]elemetry/OpenTelemetry/g' $($FIND -type f -print)
# No CRLF line endings.
"${SED[@]}" 's/\r$//' $($FIND -type f -print)
# No trailing spaces.
"${SED[@]}" 's/ \+$//' $($FIND -type f -print)

# If not overridden, try to use clang-format-8 or clang-format.
if [[ -z "$CLANG_FORMAT" ]]; then
  CLANG_FORMAT=clang-format
  if which clang-format-8 >/dev/null; then
    CLANG_FORMAT=clang-format-8
  fi
fi

$CLANG_FORMAT -version
$CLANG_FORMAT -i -style=file \
  $($FIND -name '*.cc' -print -o -name '*.h' -print)

if which cmake-format >/dev/null; then
  echo "Running cmake-format $(cmake-format --version 2>&1)."
  cmake-format -i $($FIND -name 'CMakeLists.txt' -print)
else
  echo "Can't find cmake-format. It can be installed with:"
  echo "  pip install --user cmake_format"
fi

if which buildifier >/dev/null; then
  echo "Running buildifier."
  buildifier $($FIND -name WORKSPACE -print -o -name BUILD -print -o \
    -name '*.bzl' -print)
else
  echo "Can't find buildifier. It can be installed with:"
  echo "  go get github.com/bazelbuild/buildtools/buildifier"
fi
