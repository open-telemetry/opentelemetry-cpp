#!/bin/bash
set -euxo pipefail

cp LICENSE out/LICENSE.md
mkdir out/lib/extra
cp /usr/local/lib/libproto*.* out/lib/extra/
cp /usr/local/opt/curl/lib/libcurl*.* out/lib/extra/
cp /usr/local/lib/libssl*.* out/lib/extra/
cp /usr/local/lib/libcrypto*.* out/lib/extra/
cp /usr/local/lib/libz*.* out/lib/extra/
cp /usr/local/lib/absl_*.* out/lib/extra/

pushd out
zip -r "../opentelemetry-cpp-mac-${OPENTELEMETRY_CPP_LIBARCH}.zip" *
popd # out