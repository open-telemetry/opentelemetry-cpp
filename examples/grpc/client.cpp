#include "tracer_common.h"
#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "messages.grpc.pb.h"

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

    opentelemetry::trace::StartSpanOptions options;
    options.kind = opentelemetry::trace::SpanKind::kClient;

    std::string span_name = "GreeterClient/Greet";
    auto span             = get_tracer("grpc")->StartSpan(span_name,
                                              {{"rpc.system", "grpc"},
                                               {"rpc.service", "grpc-example.GreetService"},
                                               {"rpc.method", "Greet"},
                                               {"net.peer.ip", ip},
                                               {"net.peer.port", port}},
                                              options);

    auto scope = get_tracer("grpc-client")->WithActiveSpan(span);

  auto current_ctx = opentelemetry::context::RuntimeContext::GetCurrent();
  GrpcClientCarrier carrier(&context);
  auto prop = opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
  prop->Inject(carrier, current_ctx);

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
  // set global propagator
  opentelemetry::context::propagation::GlobalTextMapPropagator::SetGlobalPropagator(
      nostd::shared_ptr<opentelemetry::context::propagation::TextMapPropagator>(
          new opentelemetry::trace::propagation::HttpTraceContext()));
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
