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
    std::string peer_name                         = context->peer();
    std::vector<std::string> peer_name_attributes = split(peer_name, ':');

    // Create a text map carrier and fill it with other headers. gRPC runs on
    // HTTP 2, so we add all the HTTP headers that we just extracted
    gRPCMapCarrier carrier;
    carrier.gRPCMapCarrier::Set("net.ip.version", peer_name_attributes.at(0));
    carrier.gRPCMapCarrier::Set("net.peer.ip", peer_name_attributes.at(1));
    carrier.gRPCMapCarrier::Set("net.peer.port", peer_name_attributes.at(2));

    // Build a context and add a k-v pair for the current span, so OpenTelemetry
    // can know the causal relationship between spans, and can infer the parent
    // -> child hierarchy of spans
    opentelemetry::context::Context ctx1 = opentelemetry::context::Context{"current-span", span};
    // Because contexts are immutable, we build another one and extract the contents
    // of our carrier with extra header information, and ctx1 which has our current span
    opentelemetry::context::Context ctx2 = propagator->Extract(carrier, ctx1);
    // Build another carrier to hold the combination of all of our span data
    gRPCMapCarrier carrier2;
    propagator->Inject(carrier2, ctx2);

    std::string req = request->request();
    std::cout << std::endl << "grpc_client says: " << req << std::endl;
    std::string message = "The pleasure is mine.";
    response->set_response(message);

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
