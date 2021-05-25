# Snapshot of OpenTelemetry `nostd::variant` implementation borrowed from `absl::variant`

This document explains how to refresh the local snapshot of OpenTelemetry `absl::variant`
snapshot used by header-only OpenTelemetry API. Start with cloning Abseil and check out
using Release tag. Latest tag at the time of this writing is `20210324.1`.

Example:

```console
git clone https://github.com/abseil/abseil-cpp
cd abseil-cpp
git checkout 20210324.1
```

`api/include/opentelemetry/nostd/absl` directory contains the necessary set of classes.

Perform side-by-side comparison of two directories to understand the basic differences
between the two:
- `abseil-cpp/absl` - contains checked-out Abseil source code.
- `opentelemetry-cpp/api/include/opentelemetry/nostd/absl` - contains OpenTemetry local snapshot.

Minimum required set of files for header-only implementation of `absl::variant` listed below.

Example contents of `api/include/opentelemetry/nostd/absl`:

```console
mgolovanov@xps:/mnt/c/work/opentelemetry-cpp.main/api/include/opentelemetry/nostd/absl$ find . -name \*\.h -print
./base/attributes.h
./base/config.h
./base/internal/identity.h
./base/internal/inline_variable.h
./base/internal/invoke.h
./base/macros.h
./base/optimization.h
./base/options.h
./base/policy_checks.h
./base/port.h
./meta/type_traits.h
./types/bad_variant_access.h
./types/internal/variant.h
./types/variant.h
./utility/utility.h
```

Other classes are unnecessary for `nostd::variant` (`absl::variant`) to work.

Perform search and replace of all `ABSL_` to `OTABSL_`. This is required in order to avoid clashes
with another instance of Abseil library used by gRPC / Protobuf code in OTLP exporter. Without this
renaming the version of Abseil used by gRPC may not necessarily match the version of the ABI-stable
snasphot used by OpenTelemetry. Abseil does not allow to include two different versions of the
library in one compilation unit. That happens even if both versions have been configured to use
a different inline namespace. Thus, we have to ensure that all macros have been renamed too.

Renaming is necessary to avoid the conflict between the two different versions of Abseil.

OpenTelemetry does not expose Abseil library as `absl::` namespace. It is kept private. Customers
must use OpenTelemetry `nostd::` namespace classes only. `nostd::variant` must be used for variant.

Customers should not use `absl::variant` directly. Depending on build configuration, OpenTelemetry
C++ SDK may also use either its own snapshot of Abseil, or standard library (e.g. STL) of
`std::variant`, or share the implementation of variant with the gRPC library. In all of these cases
it is important to continue using `nostd::` classes for code portability reasons.

# Script to perform namespace renaming from `ABSL_` to `OTABSL_`

Change to directory that contains the `abseil-cpp` snapshot.

Review the script below. This process irreversibely alters the contents of Abseil snapshot!

Run the following commands in Unix shell:

```console
cd abseil-cpp
find . -type f -name "*.cc" -delete
grep -lr --exclude-dir=".git" -e "ABSL_" . | xargs sed -i '' -e 's/ABSL_/OTABSL_/g'
```

Next, use any visual diff tool to compare your snapshot versus OpenTelemetry snapshot.
[Beyond Compare](https://www.scootersoftware.com/download.php) is a good cross-platform tool
to use for that purpose.

Carefully compare all files line-by-line. You'd notice that after the namespace renaming
is done, there are still several places of code that require manual patching:
- use relative path to Abseil in OpenTelemetry API, e.g. `#include "../../base/config.h"`.
- use `absl::OTABSL_OPTION_INLINE_NAMESPACE_NAME::` before `base_internal`.
- custom header-only `[[noreturn]] static inline void ThrowBadVariantAccess()` implementation.
- custom `options.h` settings for Abseil to avoid using `std::` implementation.
- `#define OTABSL_OPTION_INLINE_NAMESPACE_NAME otel_v1` for private ABI-stable inline namespace.

The goal of these patches is to completely isolate the local snapshot of Abseil from any other
instance of Abseil library that may be present in other components, e.g. in OTLP exporter gRPC /
protobuf code. Please make sure you respect the prior patches. Once you applied the same patches
on top of new snapshot, please run any regression and sanity tests as-required. In particular,
please run all tests that require OTLP Exporter. OTLP Exporter depends on gRPC library that may
use its own private and different version of Abseil C++ library. We would like to avoid any
collisions with that other instance of the library.

In case if new Abseil library snapshot is significantly different from the current version
used by OpenTelemetry SDK, then it may be ABI-incompatible. You need to run
[ABI Compliance Checker](https://lvc.github.io/abi-compliance-checker/) tool to verify your
compiled DLL or Shared Libraries for compliance. If ABI compliance issues were discovered,
then increment the version of inline namespace from `otel_v1` to `otel_v1_x`. Once all tests
pass, conclude the merge / integration of the new snapshot. Please avoid unnecessarily
updating the Abseil snapshot unless there is a significant compiler issue or security bug
addressed in the mainline. It is not expected for the snapshot to be taken more often than
once in several years.

Enjoy the private snapshot of Abseil `absl::variant` included in OpenTelemetry API. This private
snapshot does not collide with any other instance of the Abseil library used elsewhere in your
project. This snapshot provides a long-term ABI compatibility guarantee for OpenTelemetry C++
API surface for years to come until the next major API version upgrade gets released.
