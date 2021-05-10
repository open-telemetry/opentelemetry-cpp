#include "grpc_foo_lib/foo_split.h"
#include "grpc_foo_lib/grpc_map_carrier.h"
#include "messages.grpc.pb.h"
#include "tracer_common.h"

#include <grpcpp/grpcpp.h>
#include <iostream>
#include <string>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

using grpc_example::Greeter;
using grpc_example::GreetRequest;
using grpc_example::GreetResponse;

namespace
{
class GreeterClient
{
public:
  GreeterClient(std::shared_ptr<Channel> channel) : stub_(Greeter::NewStub(channel)) {}

  std::string Greet(std::string ip, uint16_t port)
  {
    // Build gRPC Context objects and protobuf message containers
    GreetRequest request;
    GreetResponse response;
    ClientContext context;
    request.set_request("Nice to meet you!");
    // See tracer_common.h for this function
    auto propagator = get_propagator();

    opentelemetry::trace::StartSpanOptions options;
    options.kind = opentelemetry::trace::SpanKind::kClient;
    // Spans are only as useful as the information that you put in them.
    // Even though this is a client service, it can still be useful to add
    // as much data as possible. We add attributes for peer ip and port, both
    // because it is required per OpenTelemetry's rpc semantic conventions,
    // and because it could still be useful to a debugger in the future.
    std::string span_name = "GreeterClient/Greet";
    auto span             = get_tracer("grpc")->StartSpan(span_name,
                                              {{"rpc.system", "grpc"},
                                               {"rpc.service", "grpc-example.GreetService"},
                                               {"rpc.method", "Greet"},
                                               {"net.peer.ip", ip},
                                               {"net.peer.port", port}},
                                              options);
    auto scope            = get_tracer("grpc")->WithActiveSpan(span);
    // Text map-style carrier for the propagator. Include other relevant information here.
    gRPCMapCarrier carrier;
    carrier.gRPCMapCarrier::Set("http.header.stub", "temporarily-stubbed");
    // The current runtime context has information about the currently active span.
    // We need to add that to our carrier so that the server can extract it later,
    // enabling the span hierarchy.
    auto current_ctx = opentelemetry::context::RuntimeContext::GetCurrent();
    propagator->Inject(carrier, current_ctx);
    // Send request to server
    Status status = stub_->Greet(&context, request, &response);
    if (status.ok())
    {
      span->SetStatus(opentelemetry::trace::StatusCode::kOk);
      span->SetAttribute("rpc.grpc.status_code", status.error_code());
      // Make sure to end your spans!
      span->End();
      return response.response();
    }
    else
    {
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
      span->SetStatus(opentelemetry::trace::StatusCode::kError);
      span->SetAttribute("rpc.grpc.status_code", status.error_code());
      // Make sure to end your spans!
      span->End();
      return "RPC failed";
    }
  }

private:
  std::unique_ptr<Greeter::Stub> stub_;
};  // GreeterClient class

void RunClient(uint16_t port)
{
  GreeterClient greeter(
      grpc::CreateChannel("0.0.0.0:" + std::to_string(port), grpc::InsecureChannelCredentials()));
  std::string response = greeter.Greet("0.0.0.0", port);
  std::cout << "grpc_server says: " << response << std::endl;
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
  RunClient(port);
  return 0;
}
