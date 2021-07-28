// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <opentelemetry/ext/http/client/http_client.h>
#include <opentelemetry/version.h>

#include <thrift/transport/TVirtualTransport.h>

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

class THttpTransport : public apache::thrift::transport::TVirtualTransport<THttpTransport>
{
public:
  THttpTransport(std::string endpoint, ext::http::client::Headers extra_headers);
  ~THttpTransport() override;

  bool isOpen() const override;

  uint32_t read(uint8_t *buf, uint32_t len);

  void write(const uint8_t *buf, uint32_t len);

  bool sendSpans();

private:
  std::string endpoint;
  ext::http::client::Headers headers;
  std::shared_ptr<ext::http::client::HttpClientSync> client;
  std::vector<uint8_t> request_buffer;
};

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
