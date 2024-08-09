#!/bin/bash
set -euxo pipefail

mkdir out/lib/extra
cp /usr/local/lib/libprotobuf*.* out/lib/extra/
cp /usr/local/opt/curl/lib/libcurl*.* out/lib/extra/
cp /usr/local/lib/libssl*.* out/lib/extra/
# cp /usr/local/opt/openssl/lib/lib*.* out/lib/extra/ # Includes libcrypto
cp /usr/local/lib/libcrypto*.* out/lib/extra/
cp /usr/local/lib/libz*.* out/lib/extra/
# cp /usr/local/opt/zstd/lib/lib*.* out/lib/extra/

pushd out
zip -r "../opentelemetry-cpp-mac-${OPENTELEMETRY_CPP_LIBARCH}.zip" *
popd # out