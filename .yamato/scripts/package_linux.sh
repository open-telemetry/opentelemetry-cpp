#!/bin/bash
set -euxo pipefail

cp LICENSE out/LICENSE.md
mkdir out/lib/extra
cp /usr/local/lib/libprotobuf*.* out/lib/extra/
find /usr/lib -name 'libcurl.*' -exec cp '{}' out/lib/extra/ ';'
find /usr/lib -name 'libssl.*' -exec cp '{}' out/lib/extra/ ';'
find /usr/lib -name 'libcrypto.*' -exec cp '{}' out/lib/extra/ ';'
find /usr/lib -name 'libz.*' -exec cp '{}' out/lib/extra/ ';'
find /usr/lib -name 'libnghttp2.*' -exec cp '{}' out/lib/extra/ ';'
find /usr/local/lib -name 'absl_*.*' -exec cp '{}' out/lib/extra/ ';'

pushd out
zip -r "../opentelemetry-cpp-lin-${OPENTELEMETRY_CPP_LIBARCH}.zip" *
popd # out