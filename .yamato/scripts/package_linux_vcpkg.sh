#!/bin/bash
set -euxo pipefail

cp LICENSE out/LICENSE.md
mkdir out/lib/extra
cp tools/vcpkg/installed/x64-linux-release/lib/*.* out/lib/extra/

pushd out
zip -r "../opentelemetry-cpp-lin-${OPENTELEMETRY_CPP_LIBARCH}.zip" *
popd # out