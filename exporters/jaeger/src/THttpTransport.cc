// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "THttpTransport.h"
#include "opentelemetry/ext/http/client/http_client_factory.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

THttpTransport::THttpTransport(std::string endpoint, const std::vector<HttpHeader> &extra_headers)
    : endpoint(std::move(endpoint)),
      headers({{"Content-Type", "application/vnd.apache.thrift.binary"}}),
      client(ext::http::client::HttpClientFactory::CreateSync())
{
  for (const auto &header : extra_headers)
  {
    headers.insert({header.key, header.value});
  }
}

THttpTransport::~THttpTransport() {}

bool THttpTransport::isOpen() const
{
  return true;
}

uint32_t THttpTransport::read(uint8_t *buf, uint32_t len)
{
  (void)buf;
  (void)len;
  return 0;
}

void THttpTransport::write(const uint8_t *buf, uint32_t len)
{
  request_buffer.insert(request_buffer.end(), buf, buf + len);
}

bool THttpTransport::sendSpans()
{
  auto result = client->Post(endpoint, request_buffer, headers);
  request_buffer.clear();
  if (!result)
  {
    return false;
  }

  if (result.GetResponse().GetStatusCode() >= 400)
  {
    return false;
  }

  return true;
}

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
