#pragma once
#include "http_client.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace http
{
namespace client
{

class NoopResponse : public Response
{
public:
  const Body &GetBody() const noexcept override { return body; }

  bool ForEachHeader(
      nostd::function_ref<bool(nostd::string_view name, nostd::string_view value)> callable) const
      noexcept override
  {
    return true;
  }

  bool ForEachHeader(
      const nostd::string_view &key,
      nostd::function_ref<bool(nostd::string_view name, nostd::string_view value)> callable) const
      noexcept override
  {
    return true;
  }

  StatusCode GetStatusCode() const noexcept override { return 0; }

private:
  Body body;
};
}  // namespace client
}  // namespace http
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE