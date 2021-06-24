// Make sure to include GRPC headers first because otherwise Abseil may create
// ambiguity with `nostd::variant` if compiled with Visual Studio 2015. Other
// modern compilers are unaffected.
#include <grpcpp/grpcpp.h>
#include "messages.grpc.pb.h"

#include "opentelemetry/trace/semantic_conventions.h"
#include "tracer_common.h"
#include <iostream>
#include <memory>
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

using namespace opentelemetry::trace;
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
                                              {{OTEL_CPP_GET_ATTR(AttrRpcSystem), "grpc"},
                                               {OTEL_CPP_GET_ATTR(AttrRpcService), "grpc-example.GreetService"},
                                               {OTEL_CPP_GET_ATTR(AttrRpcMethod), "Greet"},
                                               {OTEL_CPP_GET_ATTR(AttrNetPeerIp), ip},
                                               {OTEL_CPP_GET_ATTR(AttrNetPeerPort), port}},
                                              options);

    auto scope = get_tracer("grpc-client")->WithActiveSpan(span);

    // inject current context to grpc metadata
    auto current_ctx = opentelemetry::context::RuntimeContext::GetCurrent();
    GrpcClientCarrier carrier(&context);
    auto prop = opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
    prop->Inject(carrier, current_ctx);

    // Send request to server
    Status status = stub_->Greet(&context, request, &response);
    if (status.ok())
    {
      span->SetStatus(opentelemetry::trace::StatusCode::kOk);
      span->SetAttribute(OTEL_CPP_GET_ATTR(AttrRpcGrpcStatusCode), status.error_code());
      // Make sure to end your spans!
      span->End();
      return response.response();
    }
    else
    {
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
      span->SetStatus(opentelemetry::trace::StatusCode::kError);
      span->SetAttribute(OTEL_CPP_GET_ATTR(AttrRpcGrpcStatusCode), status.error_code());
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
