@echo off
cd /d %~dp0
bazel --quiet --noclient_debug run "@multitool//tools/multitool:cwd" -- %*
