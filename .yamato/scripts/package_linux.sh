#!/bin/bash
set -euxo pipefail

cp LICENSE out/LICENSE.md
mkdir out/lib/extra
cp /usr/local/lib/libprotobuf*.* out/lib/extra/
cp /usr/lib/x86_64-linux-gnu/libcurl.* out/lib/extra/
cp /usr/lib/x86_64-linux-gnu/libssl.* out/lib/extra/
cp /usr/lib/x86_64-linux-gnu/libcrypto.* out/lib/extra/
cp /usr/lib/x86_64-linux-gnu/libz.* out/lib/extra/
cp /usr/lib/x86_64-linux-gnu/libnghttp2.* out/lib/extra/
cp /usr/local/lib/libabsl_*.* out/lib/extra/

pushd out
zip -r "../opentelemetry-cpp-lin-${OPENTELEMETRY_CPP_LIBARCH}.zip" *
popd # out