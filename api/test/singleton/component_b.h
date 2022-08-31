// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// component_b is compiled with visibility("hidden"),
// so make the entry point visible

__attribute__((visibility("default")))
void do_something_in_b();
