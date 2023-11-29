// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

// Make sure to include GRPC headers first because otherwise Abseil may create
// ambiguity with `nostd::variant` if compiled with Visual Studio 2015. Other
// modern compilers are unaffected.
#include <grpcpp/grpcpp.h>
#ifdef BAZEL_BUILD
#  include "examples/grpc/protos/messages.grpc.pb.h"
#else
#  include "messages.grpc.pb.h"
#endif

#include <iostream>
#include <memory>
#include <string>

#include "opentelemetry/trace/semantic_conventions.h"
#include "tracer_common.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

using grpc_example::Greeter;
using grpc_example::GreetRequest;
using grpc_example::GreetResponse;

namespace
{
namespace context = opentelemetry::context;
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

    StartSpanOptions options;
    options.kind = SpanKind::kClient;

    std::string span_name = "GreeterClient/Greet";
    auto span             = get_tracer("grpc")->StartSpan(
        span_name,
        {{SemanticConventions::kRpcSystem, "grpc"},
         {SemanticConventions::kRpcService, "grpc-example.GreetService"},
         {SemanticConventions::kRpcMethod, "Greet"},
         {SemanticConventions::kNetworkPeerAddress, ip},
         {SemanticConventions::kNetworkPeerPort, port}},
        options);

    auto scope = get_tracer("grpc-client")->WithActiveSpan(span);

    // inject current context to grpc metadata
    auto current_ctx = context::RuntimeContext::GetCurrent();
    GrpcClientCarrier carrier(&context);
    auto prop = context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
    prop->Inject(carrier, current_ctx);

    // Send request to server
    Status status = stub_->Greet(&context, request, &response);
    if (status.ok())
    {
      span->SetStatus(StatusCode::kOk);
      span->SetAttribute(SemanticConventions::kRpcGrpcStatusCode, status.error_code());
      // Make sure to end your spans!
      span->End();
      return response.response();
    }
    else
    {
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
      span->SetStatus(StatusCode::kError);
      span->SetAttribute(SemanticConventions::kRpcGrpcStatusCode, status.error_code());
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
  InitTracer();
  // set global propagator
  context::propagation::GlobalTextMapPropagator::SetGlobalPropagator(
      opentelemetry::nostd::shared_ptr<context::propagation::TextMapPropagator>(
          new propagation::HttpTraceContext()));
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
  CleanupTracer();
  return 0;
}
