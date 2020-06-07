#!/bin/sh
export MACOSX_DEPLOYMENT_TARGET=${MACOSX_DEPLOYMENT_TARGET:-10.11}
curl -L -O https://github.com/phracker/MacOSX-SDKs/releases/download/MacOSX10.11.sdk/MacOSX10.11.sdk.tar.xz
tar -xf MacOSX${MACOSX_DEPLOYMENT_TARGET}.sdk.tar.xz -C /Library/Developer/CommandLineTools/SDKs/
