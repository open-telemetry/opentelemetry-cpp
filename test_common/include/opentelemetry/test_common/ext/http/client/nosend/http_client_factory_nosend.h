// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/test_common/ext/http/client/nosend/http_client_nosend.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace test_common
{
namespace ext
{
namespace http
{
namespace client
{
namespace nosend
{

class HttpClientFactoryNosend : public opentelemetry::ext::http::client::HttpClientFactory
{
public:
  std::shared_ptr<opentelemetry::ext::http::client::HttpClient> Create() override
  {
    return std::make_shared<opentelemetry::ext::http::client::nosend::HttpClient>();
  }

  std::shared_ptr<opentelemetry::ext::http::client::HttpClient> Create(
      const std::shared_ptr<opentelemetry::sdk::common::ThreadInstrumentation> &) override
  {
    return std::make_shared<opentelemetry::ext::http::client::nosend::HttpClient>();
  }

  std::shared_ptr<opentelemetry::ext::http::client::HttpClientSync> CreateSync() override
  {
    return nullptr;
  }
};

}  // namespace nosend
}  // namespace client
}  // namespace http
}  // namespace ext
}  // namespace test_common
OPENTELEMETRY_END_NAMESPACE
