##  Building and running tests as a developer
CI tests can be run on docker by invoking the script `./ci/run_docker.sh ./ci/do_ci.sh <TARGET>` where the targets are
* `cmake.test` build cmake targets and run tests
* `bazel.test` build bazel targets and run tests
Additionally, `./ci/run_docker.sh` can be invoked with no arguments to get a docker shell where tests
can be run manually.
