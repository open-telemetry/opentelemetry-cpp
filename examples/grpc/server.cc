#include "grpc_foo_lib/foo_split.h"
#include "grpc_foo_lib/grpc_map_carrier.h"
#include "messages.grpc.pb.h"
#include "tracer_common.h"
#include "opentelemetry/trace/span_context_kv_iterable_view.h"

#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <map>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;

using grpc_example::Greeter;
using grpc_example::GreetRequest;
using grpc_example::GreetResponse;

using Span = opentelemetry::trace::Span;
using SpanContext = opentelemetry::trace::SpanContext;

namespace
{
class GreeterServer final : public Greeter::Service
{
public:
  Status Greet(ServerContext *context,
               const GreetRequest *request,
               GreetResponse *response) override
  {
    // Create a SpanOptions object and set the kind to Server to inform OpenTel.
    opentelemetry::trace::StartSpanOptions options;
    options.kind = opentelemetry::trace::SpanKind::kServer;
    // Create a text map carrier for our propagator. This carrier holds all of our
    // information that needs to be propagated. We instantiate our carrier here as
    // in your systems you will probably have information to fill it with before 
    // even starting your span, maybe your client's http headers. There is no way
    // to get any carrier-relevant information at this point without doing work that
    // we want to capture in a span, but we include the definition at this point to fit
    // real-life scenarios
    std::map<std::string, std::string> m;
    m.insert(std::pair<std::string, std::string>("1", "2"));
    const gRPCMapCarrier<std::map<std::string, std::string>> carrier((std::map<std::string, std::string> &) m);
    auto prop = opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
    auto current_ctx = opentelemetry::context::RuntimeContext::GetCurrent();
    auto new_context = prop->Extract(carrier, current_ctx);
    auto s = GetSpanFromContext(new_context);
    opentelemetry::nostd::span<const uint8_t, 8> sp = s->GetContext().span_id().Id(); 
    for(const uint8_t &e : sp) {
      std::cout << unsigned(e) << ' ';
    }
    std::cout << '\n';
    options.parent   = s->GetContext();

    // Build a new span from a tracer, start, and activate it. When adding
    // attributes to spans, be sure to follow the OpenTelemetry Semantic
    // Conventions that are relevant to your project as closely as possible
    // Some protocols/patterns that have specified conventions:
    // RPC, HTTP, AWS Lambda, Database, Exceptions, etc.
    std::string span_name = "GreeterService/Greet";
    auto span             = get_tracer("grpc")
                    ->StartSpan(span_name,
                                {{"rpc.system", "grpc"},
                                 {"rpc.service", "GreeterService"},
                                 {"rpc.method", "Greet"},
                                 {"rpc.grpc.status_code", 0}},
                                options);
    auto scope = get_tracer("grpc")->WithActiveSpan(span);

    // Fetch and parse whatever HTTP headers we can from the gRPC request.
    span->AddEvent("Processing client attributes");
    std::string peer_name                         = context->peer();
    std::vector<std::string> peer_name_attributes = split(peer_name, ':');

    // Fill the carrier with other headers. gRPC runs on HTTP 2, so we add all 
    // of the HTTP headers that we just extracted
    /*carrier.Set("net.ip.version", peer_name_attributes.at(0));
    carrier.Set("net.peer.ip", peer_name_attributes.at(1));
    carrier.Set("net.peer.port", peer_name_attributes.at(2));*/

    std::string req = request->request();
    std::cout << std::endl << "grpc_client says: " << req << std::endl;
    std::string message = "The pleasure is mine.";
    // Send response to client
    response->set_response(message);
    span->AddEvent("Response sent to client");

    span->SetStatus(opentelemetry::trace::StatusCode::kOk);
    // Make sure to end your spans!
    span->End();
    return Status::OK;
  }
};  // GreeterServer class

void RunServer(uint16_t port)
{
  std::string address("0.0.0.0:" + std::to_string(port));
  GreeterServer service;
  ServerBuilder builder;

  builder.RegisterService(&service);
  builder.AddListeningPort(address, grpc::InsecureServerCredentials());

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on port: " << address << std::endl;
  server->Wait();
  server->Shutdown();
}
}  // namespace

int main(int argc, char **argv)
{
  initTracer();
  constexpr uint16_t default_port = 8800;
  uint16_t port;
  if (argc > 1)
  {
    port = atoi(argv[1]);
  }
  else
  {
    port = default_port;
  }

  RunServer(port);
  return 0;
}
