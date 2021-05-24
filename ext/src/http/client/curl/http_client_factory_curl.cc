// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/ext/http/client/curl/http_client_curl.h"
#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"

std::shared_ptr<opentelemetry::ext::http::client::HttpClient>
opentelemetry::ext::http::client::HttpClientFactory::Create()
{
  return std::make_shared<opentelemetry::ext::http::client::curl::HttpClient>();
}

std::shared_ptr<opentelemetry::ext::http::client::HttpClientSync>
opentelemetry::ext::http::client::HttpClientFactory::CreateSync()
{
  return std::make_shared<opentelemetry::ext::http::client::curl::HttpClientSync>();
}