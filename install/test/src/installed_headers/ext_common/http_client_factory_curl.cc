// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// One translation unit per header the ext_common component installs, so a header that does not
// compile on its own fails here instead of being carried by an include above it in a shared unit.
#include <opentelemetry/ext/http/client/curl/http_client_factory_curl.h>
