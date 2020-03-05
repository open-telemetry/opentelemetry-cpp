#pragma once

#define OPENTELEMETRY_VERSION "0.0.1"
#define OPENTELEMETRY_ABI_VERSION "0"

// clang-format off
#define BEGIN_OPENTELEMETRY_ABI_NAMESPACE \
  inline namespace v##OPENTELEMETRY_ABI_VERSION { 

#define END_OPENTELEMETRY_ABI_NAMESPACE \
  }
// clang-format on
