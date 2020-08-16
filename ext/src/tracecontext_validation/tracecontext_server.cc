#include "opentelemetry/ext/tracecontext_validation/tracecontext_server.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace validation
{
void TraceContextServer::SetClientManager(HttpClients &http_clients) { clients = http_clients; }

void TraceContextServer::InitializeCallBack(FileHttpServer &server) { server[test_protocol_] = SendRequestBack; }
} // namespace validation
} // namespace ext
OPENTELEMETRY_END_NAMESPACE
