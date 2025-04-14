// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdio.h>

#include <opentelemetry/ext/grpc/grpc_config.h>
#include <grpc/grpc.h>

using namespace opentelemetry::ext::grpc;

TEST(GrpcConfigTests, OverridesToString)
{
  grpc_init();
  printf("\n%s\n\n", OverridesToString().c_str());
}

TEST(GrpcConfigTests, ResetThenOverridesToString)
{
  grpc_init();
  ResetOverrides();
  printf("\n%s\n\n", OverridesToString().c_str());
}
