// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// file_http_server.h is an installed public header that no other translation unit in the
// repository includes, so nothing compiled it and a build error could sit there unnoticed (the
// _WIN32 branch did exactly that). This translation unit exists only to compile the header, so
// CI builds the POSIX branch here and the Windows branch on the Windows runners. It has no
// main() and is never run.
#include "opentelemetry/ext/http/server/file_http_server.h"  // IWYU pragma: keep
