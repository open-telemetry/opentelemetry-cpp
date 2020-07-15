#!/bin/bash
DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
export PATH=$DIR:$PATH
WORKSPACE_ROOT=$DIR/..

# Run tests for each compiler $C - each build configuration $D
pushd $WORKSPACE_ROOT/out
for C in `find . -mindepth 1 -maxdepth 1 -type d`; do
  pushd $C
  for D in `find . -mindepth 1 -maxdepth 1 -type d`; do
    pushd $D
    export OUTDIR=`pwd`
    echo Running tests in $OUTDIR
    run-tests.sh $OUTDIR > $OUTDIR/test_results.log
    popd
  done
  popd
done
popd
