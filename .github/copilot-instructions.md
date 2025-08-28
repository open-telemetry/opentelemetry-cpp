# OpenTelemetry C++

OpenTelemetry C++ is a comprehensive telemetry SDK providing APIs and
implementations for traces, metrics, and logs. It supports both CMake and Bazel
build systems and runs on Linux, macOS, and Windows with modern C++ compilers
(C++14/17/20).

Always reference these instructions first and fallback to search or bash
commands only when you encounter unexpected information that does not match the
info here.

## Working Effectively

### Bootstrap, Build, and Test the Repository

**CRITICAL: NEVER CANCEL builds or long-running commands. Set appropriate
timeouts.**

#### CMake Build (Recommended for Development)

```bash
# Install basic dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install -y build-essential cmake git pkg-config

# Configure CMake build
mkdir -p build && cd build
cmake ..
# Takes ~12 seconds. Always completes quickly.

# Build the project
make -j$(nproc)
# Takes ~3 minutes. NEVER CANCEL. Set timeout to 15+ minutes.

# Run all tests
ctest --output-on-failure
# Takes ~24 seconds. Set timeout to 5+ minutes.
```

#### CI Build (Full Validation)

```bash
# Run the full CI validation (includes additional exporters)
./ci/do_ci.sh cmake.test
# Takes ~5.2 minutes. NEVER CANCEL. Set timeout to 20+ minutes.
```

#### Bazel Build (Alternative)

```bash
# Install bazelisk (managed Bazel)
sudo ./ci/install_bazelisk.sh

# Build simple example
bazel build //examples/simple:example_simple
# Time varies. NEVER CANCEL. Set timeout to 15+ minutes.

# Run simple example
bazel-bin/examples/simple/example_simple
```

**Note**: Bazel may have network connectivity issues in some environments when
downloading the required Bazel version (7.1.1).

### Validation

Always validate your changes using these steps after making code modifications:

#### Core Validation Scenario

```bash
# 1. Build and test successfully
cd build && make -j$(nproc) && ctest --output-on-failure

# 2. Run a simple example to verify functionality
./examples/simple/example_simple
# Should output telemetry spans with service.name, trace_id, span_id

# 3. Format code properly
./tools/format.sh
# Takes ~30 seconds. Must complete without errors.

# 4. Validate with maintainer mode (CRITICAL for warnings)
./ci/do_ci.sh cmake.maintainer.sync.test
# Takes ~4-6 minutes. NEVER CANCEL. Ensures all warnings are resolved.
```

#### Required Tools for Formatting

```bash
# Install formatting dependencies
pip install cmake_format                    # For CMake files
go install github.com/bazelbuild/buildtools/buildifier@latest  # For Bazel files
# clang-format should already be available on most systems
```

### Maintainer Mode Validation

**CRITICAL**: Always run maintainer mode builds to ensure warning-free code:

```bash
# Run maintainer mode validation
./ci/do_ci.sh cmake.maintainer.sync.test

# What this does:
# - Enables -Wall -Werror -Wextra compiler flags
# - Treats all warnings as errors
# - Ensures strict code quality standards
# - Required for all contributions
```

Maintainer mode (`-DOTELCPP_MAINTAINER_MODE=ON`) is essential for catching potential issues that would cause CI failures. It enables the strictest warning levels and treats warnings as compilation errors.

### CI Integration

Always run these before committing to ensure CI will pass:

```bash
# Format all code
./tools/format.sh

# Run linting (if shellcheck available for shell scripts)
shellcheck --severity=error ci/*.sh

# CRITICAL: Validate with maintainer mode to catch all warnings
./ci/do_ci.sh cmake.maintainer.sync.test
# Takes ~4-6 minutes. Enables -Wall -Werror -Wextra for strict validation.

# Validate build with additional exporters
./ci/do_ci.sh cmake.test
```

## Common Tasks

### Building and Running Examples

Examples demonstrate OpenTelemetry functionality and validate your environment:

```bash
# Build and run simple tracing example
cd build
make example_simple
./examples/simple/example_simple

# Build and run logs example
make logs_simple_example
./examples/logs_simple/logs_simple_example

# Build and run batch processing example
make batch_example
./examples/batch/batch_example
```

### Testing Changes

```bash
# Run specific test groups
ctest -R trace                    # Run only trace tests
ctest -R metrics                  # Run only metrics tests
ctest -R logs                     # Run only logs tests

# Run tests with verbose output for debugging
ctest --verbose --output-on-failure

# Run a specific test by name
ctest -R "trace.SystemTimestampTest.Construction" --verbose
```

### Key Directories and Navigation

```text
api/                  - Public OpenTelemetry API headers
sdk/                  - SDK implementation (most business logic)
exporters/            - Output plugins (ostream, memory, etc.)
examples/             - Sample applications demonstrating usage
  ├── simple/         - Basic tracing example (start here)
  ├── logs_simple/    - Basic logging example
  ├── metrics_simple/ - Basic metrics example (may be disabled)
  └── batch/          - Batch processing example
ci/                   - CI scripts and build automation
tools/                - Development tools (formatting, etc.)
test_common/          - Shared test utilities
third_party/          - External dependencies
```

### Important Files

- `CMakeLists.txt` - Main CMake configuration
- `WORKSPACE` - Bazel workspace configuration
- `third_party_release` - Dependency version specifications
- `ci/do_ci.sh` - Main CI script with build targets
- `tools/format.sh` - Code formatting script
- `.github/workflows/ci.yml` - GitHub Actions CI configuration

## Build Targets and Options

### CI Script Targets (./ci/do_ci.sh)

```bash
cmake.test                       # Standard CMake build with exporters (~5.2 min)
cmake.maintainer.sync.test       # Maintainer mode: -Wall -Werror -Wextra (~4-6 min)
cmake.maintainer.async.test      # Maintainer mode with async export enabled
cmake.maintainer.cpp11.async.test # Maintainer mode with C++11
cmake.maintainer.abiv2.test      # Maintainer mode with ABI v2
cmake.legacy.test                # GCC 4.8 compatibility testing
cmake.c++20.test                # C++20 standard testing
bazel.test                      # Standard Bazel build and test
format                          # Run formatting tools
code.coverage                   # Build with coverage analysis
```

### CMake Configuration Options

Key options you can pass to `cmake ..`:

```bash
-DWITH_EXAMPLES=ON           # Build examples (default ON)
-DWITH_PROMETHEUS=ON         # Enable Prometheus exporter
-DWITH_ZIPKIN=ON            # Enable Zipkin exporter
-DWITH_OTLP_GRPC=ON         # Enable OTLP gRPC exporter
-DWITH_OTLP_HTTP=ON         # Enable OTLP HTTP exporter
-DBUILD_TESTING=ON          # Build tests (default ON)
-DCMAKE_BUILD_TYPE=Debug    # Debug build
```

## Timing Expectations

**CRITICAL**: These are measured times. Always set longer timeouts to prevent
premature cancellation.

| Operation | Measured Time | Recommended Timeout |
|-----------|---------------|-------------------|
| CMake configure | 12 seconds | 2 minutes |
| CMake build (parallel) | 3 minutes | 15 minutes |
| Test execution (ctest) | 24 seconds | 5 minutes |
| CI cmake.test | 5.2 minutes | 20 minutes |
| CI cmake.maintainer.sync.test | 4-6 minutes | 20 minutes |
| Format script | 17 seconds | 2 minutes |
| Bazel build | Varies | 15+ minutes |

**NEVER CANCEL** any build or test operation. Build systems may appear to hang
but are typically downloading dependencies or performing intensive compilation.

## Troubleshooting

### Common Issues

**Build Fails**:

- Ensure all dependencies installed:
  `sudo apt-get install build-essential cmake git pkg-config`
- Clean build directory: `rm -rf build && mkdir build`

**Tests Fail**:

- Set `CTEST_OUTPUT_ON_FAILURE=1` for detailed test output
- Run specific failing test: `ctest -R <test_name> --verbose`

**Format Script Fails**:

- Install missing tools: `pip install cmake_format` and buildifier via Go
- Check clang-format version: should be 14+ (18+ preferred)

**Bazel Issues**:

- Network connectivity may prevent Bazel version download
- Use CMake as primary build system for development
- Check `.bazelversion` file for required version (7.1.1)

### Network/Connectivity Issues

Some tools require internet access:

- Bazel downloading specific version
- Third-party dependencies during first build
- CI scripts pulling Docker images for benchmarks

For offline development, use CMake with pre-installed dependencies.

## Repository Structure Summary

**Core Components**:

- **API**: Header-only library in `api/` for instrumentation
- **SDK**: Implementation in `sdk/` with resource detection, processors, exporters
- **Exporters**: Output backends in `exporters/` (console, memory, Prometheus, etc.)

**Development Workflow**:

1. Make changes to relevant component
2. Build and test: `cd build && make -j$(nproc) && ctest`
3. Run example: `./examples/simple/example_simple`
4. Format code: `./tools/format.sh`
5. Validate warnings: `./ci/do_ci.sh cmake.maintainer.sync.test`
6. Final validation: `./ci/do_ci.sh cmake.test`

**Key Standards**:

- C++14 minimum, C++17/20 supported
- Google C++ Style Guide for naming
- Automatic formatting via clang-format
- Comprehensive test coverage with GoogleTest
