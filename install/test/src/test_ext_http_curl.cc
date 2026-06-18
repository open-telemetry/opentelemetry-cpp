// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <opentelemetry/ext/http/client/curl/http_client_factory_curl.h>
#include <opentelemetry/ext/http/client/http_client.h>

TEST(ExtHttpCurlInstall, HttpClient)
{
  auto client =
      std::make_shared<opentelemetry::ext::http::client::curl::HttpCurlClientFactory>()->Create();
  ASSERT_TRUE(client != nullptr);
}