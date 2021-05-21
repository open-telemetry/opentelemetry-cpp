// Copyright (c) 2021 OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#define OPENTELEMETRY_STRINGIFY(S) OPENTELEMETRY_STRINGIFY_(S)
#define OPENTELEMETRY_STRINGIFY_(S) #S

#define OPENTELEMETRY_CONCAT(A, B) OPENTELEMETRY_CONCAT_(A, B)
#define OPENTELEMETRY_CONCAT_(A, B) A##B
