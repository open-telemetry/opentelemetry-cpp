// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/ext/http/client/http_client_factory.h"
#include "opentelemetry/ext/http/client/curl/http_client_curl.h"
#include "opentelemetry/ext/http/client/http_client.h"
#ifdef ENABLE_TEST
#  include "opentelemetry/ext/http/client/nosend/http_client_nosend.h"
#endif
namespace http_client = opentelemetry::ext::http::client;

std::shared_ptr<http_client::HttpClient> http_client::HttpClientFactory::Create()
{
#ifdef ENABLE_TEST
  return std::make_shared<http_client::nosend::HttpClient>();
#endif
  return std::make_shared<http_client::curl::HttpClient>();
}

std::shared_ptr<http_client::HttpClientSync> http_client::HttpClientFactory::CreateSync()
{
  return std::make_shared<http_client::curl::HttpClientSync>();
}