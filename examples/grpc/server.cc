#include "grpc_foo_lib/foo_split.h"
#include "grpc_foo_lib/grpc_map_carrier.h"
#include "messages.grpc.pb.h"
#include "tracer_common.h"

#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;

using grpc_example::Greeter;
using grpc_example::GreetRequest;
using grpc_example::GreetResponse;

namespace
{
class GreeterServer final : public Greeter::Service
{
public:
  Status Greet(ServerContext *context,
               const GreetRequest *request,
               GreetResponse *response) override
  {
    // Get global propagator to move spans across rpc boundaries
    auto propagator = get_propagator();
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
    gRPCMapCarrier carrier;
    // The current RuntimeContext should contain the currently active span, which is from
    // the client side. To implement the span hierarchy, we need to fetch that span from
    // the RuntimeContext so that we can set it as the parent of our new span
    auto current_ctx = opentelemetry::context::RuntimeContext::GetCurrent();
    // We want a new context that will have the information that we 'could' have placed
    // in the carrier already, as well as the RuntimeContext which has our parent-span
    // information. Contexts are immutable, so we need to make a new one
    auto new_ctx = propagator->Extract(carrier, current_ctx);
    options.parent = GetSpanFromContext(new_ctx)->GetContext();
    // Build a new span from a tracer, start, and activate it. When adding
    // attributes to spans, be sure to follow the OpenTelemetry Semantic
    // Conventions that are relevant to your project as closely as possible
    // Some protocols/patterns that have specified conventions:
    // RPC, HTTP, AWS Lambda, Database, Exceptions, etc.
    std::string span_name = "GreeterService/Greet";
    auto span             = get_tracer("grpc-server")
                    ->StartSpan(span_name,
                                {{"rpc.system", "grpc"},
                                 {"rpc.service", "GreeterService"},
                                 {"rpc.method", "Greet"},
                                 {"rpc.grpc.status_code", 0}},
                                options);
    auto scope = get_tracer("grpc-server")->WithActiveSpan(span);

    // Fetch and parse whatever HTTP headers we can from the gRPC request.
    span->AddEvent("Processing client attributes");
    std::string peer_name                         = context->peer();
    std::vector<std::string> peer_name_attributes = split(peer_name, ':');

    // Fill the carrier with other headers. gRPC runs on HTTP 2, so we add all 
    // of the HTTP headers that we just extracted
    carrier.gRPCMapCarrier::Set("net.ip.version", peer_name_attributes.at(0));
    carrier.gRPCMapCarrier::Set("net.peer.ip", peer_name_attributes.at(1));
    carrier.gRPCMapCarrier::Set("net.peer.port", peer_name_attributes.at(2));

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
