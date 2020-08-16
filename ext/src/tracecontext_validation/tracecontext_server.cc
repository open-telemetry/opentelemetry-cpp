#include "opentelemetry/ext/tracecontext_validation/tracecontext_server.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace validation
{
void SetClientManager(HttpClients &http_clients) { clients = http_clients; }
} // namespace validation
} // namespace ext
OPENTELEMETRY_END_NAMESPACE
