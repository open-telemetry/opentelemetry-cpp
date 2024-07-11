#!/bin/bash

# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Define the find command to exclude certain directories and find relevant files

# This command will exclude the following directories: same as format.sh but removing the opentelemetry-shim as well.
# - third_party
# - tools
# - .git
# - _deps
# - build
# - out
# - .vs
# - opentelemetry_logo.png
# - TraceLoggingDynamic.h:
# - opentelemetry-shim

FIND="find . -name third_party -prune -o -name opentracing-shim -prune -o -name tools -prune -o -name .git -prune -o -name _deps -prune -o -name build -prune -o -name out -prune -o -name .vs -prune -o -name opentelemetry_logo.png -prune -o -name TraceLoggingDynamic.h -prune -o -name '*.h' -o -name '*.cc' -print"

# Define the path to the compile_commands.json
COMPILE_COMMANDS_PATH="./build/compile_commands.json"

# Define the log file name
LOG_FILE="clang-tidy-output.log"


# Don't require this in the log file as the clang-tidy just want to go into system libraries and produce those warnings as well.
# This should be probably avioded with header-filter
FILTER_PATTERN="Suppressed [0-9]* warnings.*|Use -header-filter=.*|Use -system-headers.* | warnings generated."

# Check if the log file is empty or does not exist
rm -f $LOG_FILE

run_clang_tidy() {
  file=$1
  echo "Running clang-tidy on $file" >> $LOG_FILE
  clang-tidy -p=$COMPILE_COMMANDS_PATH "$file" 2>&1 | grep -Ev "$FILTER_PATTERN" | tee -a $LOG_FILE
}

NUM_PROCESSORS=$(nproc)


export -f run_clang_tidy
export COMPILE_COMMANDS_PATH
export LOG_FILE
export FILTER_PATTERN

# Find all relevant files and store them in a variable
FILES=$(eval $FIND)
NUM_FILES=$(echo "$FILES" | wc -l)


echo "Number of processors available: $NUM_PROCESSORS"
echo "Number of files to process: $NUM_FILES"

# Run clang-tidy in parallel on each file found by the find command
echo "$FILES" | xargs -P $NUM_PROCESSORS -n 1 bash -c 'run_clang_tidy "$@"' _
