#!/bin/bash
DIR="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
WORKSPACE_ROOT=$DIR/..

cd $DIR

echo Run tests for nostd containers implementation
export OUTDIR=${WORKSPACE_ROOT}/out.nostd
./run-tests.sh $OUTDIR > $OUTDIR/test_results.log

echo Run tests for STL   containers implementation
export OUTDIR=${WORKSPACE_ROOT}/out.stl
./run-tests.sh $OUTDIR > $OUTDIR/test_results.log
