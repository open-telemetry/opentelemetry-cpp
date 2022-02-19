// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/ext/http/client/nosend/http_client_nosend.h"

namespace http_client = opentelemetry::ext::http::client;

std::shared_ptr<http_client::HttpClient> http_client::HttpClientFactory::CreateNoSend()
{
  return std::make_shared<http_client::nosend::HttpClient>();
}
