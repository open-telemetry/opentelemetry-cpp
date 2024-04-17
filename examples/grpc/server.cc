// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#ifdef BAZEL_BUILD
#  include "examples/grpc/protos/messages.grpc.pb.h"
#else
#  include "messages.grpc.pb.h"
#endif

#include "opentelemetry/trace/context.h"
#include "opentelemetry/trace/semantic_conventions.h"
#include "opentelemetry/trace/span_context_kv_iterable_view.h"
#include "tracer_common.h"

#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <chrono>
#include <fstream>
#include <map>
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

using Span        = opentelemetry::trace::Span;
using SpanContext = opentelemetry::trace::SpanContext;
using namespace opentelemetry::trace;

namespace context = opentelemetry::context;

namespace
{
class GreeterServer final : public Greeter::Service
{
public:
  Status Greet(ServerContext *context,
               const GreetRequest *request,
               GreetResponse *response) override
  {
    for (auto elem : context->client_metadata())
    {
      std::cout << "ELEM: " << elem.first << " " << elem.second << "\n";
    }

    // Create a SpanOptions object and set the kind to Server to inform OpenTel.
    StartSpanOptions options;
    options.kind = SpanKind::kServer;

    // extract context from grpc metadata
    GrpcServerCarrier carrier(context);

    auto prop        = context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
    auto current_ctx = context::RuntimeContext::GetCurrent();
    auto new_context = prop->Extract(carrier, current_ctx);
    options.parent   = GetSpan(new_context)->GetContext();

    std::string span_name = "GreeterService/Greet";
    auto span             = get_tracer("grpc")->StartSpan(span_name,
                                              {{SemanticConventions::kRpcSystem, "grpc"},
                                               {SemanticConventions::kRpcService, "GreeterService"},
                                               {SemanticConventions::kRpcMethod, "Greet"},
                                               {SemanticConventions::kRpcGrpcStatusCode, 0}},
                                              options);
    auto scope            = get_tracer("grpc")->WithActiveSpan(span);

    // Fetch and parse whatever HTTP headers we can from the gRPC request.
    span->AddEvent("Processing client attributes");

    std::string req = request->request();
    std::cout << std::endl << "grpc_client says: " << req << std::endl;
    std::string message = "The pleasure is mine.";
    // Send response to client
    response->set_response(message);
    span->AddEvent("Response sent to client");

    span->SetStatus(StatusCode::kOk);
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
  InitTracer();
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
  CleanupTracer();
  return 0;
}
