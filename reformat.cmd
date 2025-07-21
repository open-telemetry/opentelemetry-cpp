@echo off
pushd "%~dp0"
bazel run @multitool//tools/buildifier:cwd -- -lint fix -mode fix -v -r .
