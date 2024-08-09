#!/bin/bash
set -euxo pipefail

mkdir out/lib/extra
cp /usr/local/lib/libprotobuf*.a out/lib/extra/
find /usr/lib -name 'libcurl.a' -exec cp '{}' out/lib/extra/ ';'
find /usr/lib -name 'libssl.a' -exec cp '{}' out/lib/extra/ ';'
find /usr/lib -name 'libcrypto.a' -exec cp '{}' out/lib/extra/ ';'
find /usr/lib -name 'libz.a' -exec cp '{}' out/lib/extra/ ';'

pushd out
zip -r "../opentelemetry-cpp-lin-${OPENTELEMETRY_CPP_LIBARCH}.zip" *
popd # out