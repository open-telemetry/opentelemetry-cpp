#!/bin/bash
set -euxo pipefail

mkdir out/lib/extra
#cp /usr/local/lib/libcurl.a out/lib/extra/
cp /usr/local/lib/libprotobuf*.a out/lib/extra/
#cp /usr/local/lib/libzstd.a out/lib/extra/

pushd out
zip -r "../opentelemetry-cpp-mac-${OPENTELEMETRY_CPP_LIBARCH}.zip" *
popd # out