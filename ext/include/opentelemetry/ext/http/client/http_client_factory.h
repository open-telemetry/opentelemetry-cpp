// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>

#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/sdk/common/thread_instrumentation.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace http
{
namespace client
{
class HttpClientFactory
{
public:
  virtual ~HttpClientFactory() = default;

  virtual std::shared_ptr<HttpClientSync> CreateSync() = 0;

  virtual std::shared_ptr<HttpClient> Create() = 0;
  virtual std::shared_ptr<HttpClient> Create(
      const std::shared_ptr<sdk::common::ThreadInstrumentation> &thread_instrumentation) = 0;
};

// Returns an HttpCurlClientFactory instance when compiled with curl support.
// Not defined otherwise — binaries that link any exporter must provide their
// own definition or use the factory/client constructor overloads exclusively.
std::shared_ptr<HttpClientFactory> GetDefaultHttpClientFactory();

}  // namespace client
}  // namespace http
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE
