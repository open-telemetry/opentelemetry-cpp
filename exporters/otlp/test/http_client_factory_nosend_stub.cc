// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <memory>

#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/test_common/ext/http/client/nosend/http_client_nosend.h"

namespace http_client = opentelemetry::ext::http::client;

namespace
{
class NosendHttpClientFactory : public http_client::HttpClientFactory
{
public:
  std::shared_ptr<http_client::HttpClient> Create() override
  {
    return std::make_shared<http_client::nosend::HttpClient>();
  }

  std::shared_ptr<http_client::HttpClient> Create(
      const std::shared_ptr<opentelemetry::sdk::common::ThreadInstrumentation> &) override
  {
    return std::make_shared<http_client::nosend::HttpClient>();
  }

  std::shared_ptr<http_client::HttpClientSync> CreateSync() override { return nullptr; }
};
}  // namespace

std::shared_ptr<http_client::HttpClientFactory> http_client::GetDefaultHttpClientFactory()
{
  static auto instance = std::make_shared<NosendHttpClientFactory>();
  return instance;
}
