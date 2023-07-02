// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "fuzztest/fuzztest.h"
#include "gtest/gtest.h"
#include "opentelemetry/sdk/common/atomic_unique_ptr.h"

using opentelemetry::sdk::common::AtomicUniquePtr;

void atomicIntTest(int input_value)
{

  opentelemetry::sdk::common::AtomicUniquePtr<int> ptr;
  bool is_null = ptr.IsNull();

  std::unique_ptr<int> x{new int{input_value}};
  bool result = ptr.SwapIfNull(x);
  is_null     = ptr.IsNull();

  if (result && is_null && x == nullptr)
  {
    // The fuzzer should detect if this code block produces crashes or other unexpected behavior
  }
}

FUZZ_TEST(AtomiPtrFuzzTests, atomicIntTest);