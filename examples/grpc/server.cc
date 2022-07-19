#ifdef BAZEL_BUILD
#  include "examples/grpc/protos/messages.grpc.pb.h"
#else
#  include "messages.grpc.pb.h"
#endif

#ifdef _WIN32
#  ifdef DELETE
#    pragma message(__FILE__ ": pollution after messages.grpc.pb.h")
#  endif
#endif

#include "opentelemetry/trace/context.h"

#ifdef _WIN32
#  ifdef DELETE
#    pragma message(__FILE__ ": pollution after context.h")
#  endif
#endif

#include "opentelemetry/trace/span_context_kv_iterable_view.h"

#ifdef _WIN32
#  ifdef DELETE
#    pragma message(__FILE__ ": pollution after span_context_kv_iterable_view.h")
#  endif
#endif

#include "tracer_common.h"

#ifdef _WIN32
#  ifdef DELETE
#    pragma message(__FILE__ ": pollution after tracer_common.h")
#  endif
#endif

#include <grpcpp/grpcpp.h>

#ifdef _WIN32
#  ifdef DELETE
#    pragma message(__FILE__ ": pollution after grpcpp.h")
#  endif
#endif

#include <grpcpp/server.h>

#ifdef _WIN32
#  ifdef DELETE
#    pragma message(__FILE__ ": pollution after server.h")
#  endif
#endif

#include <grpcpp/server_builder.h>

#ifdef _WIN32
#  ifdef DELETE
#    pragma message(__FILE__ ": pollution after server_builder.h")
#  endif
#endif

#include <grpcpp/server_context.h>

#ifdef _WIN32
#  ifdef DELETE
#    pragma message(__FILE__ ": pollution after server_context.h")
#  endif
#endif

#include <chrono>

#ifdef _WIN32
#  ifdef DELETE
#    pragma message(__FILE__ ": pollution after chrono")
#  endif
#endif

#include <fstream>

#ifdef _WIN32
#  ifdef DELETE
#    pragma message(__FILE__ ": pollution after fstream")
#  endif
#endif

#include <map>

#ifdef _WIN32
#  ifdef DELETE
#    pragma message(__FILE__ ": pollution after map")
#  endif
#endif

#include <sstream>

#ifdef _WIN32
#  ifdef DELETE
#    pragma message(__FILE__ ": pollution after sstream")
#  endif
#endif

#include <string>

#ifdef _WIN32
#  ifdef DELETE
#    pragma message(__FILE__ ": pollution after string")
#  endif
#endif

#include <thread>

#ifdef _WIN32
#  ifdef DELETE
#    pragma message(__FILE__ ": pollution after thread")
#  endif
#endif

#ifdef _WIN32
#  ifdef DELETE
// winnt.h defines DELETE
// Causes a build error with FaasDocumentOperationValues::DELETE
#    pragma message(__FILE__ ": removing define on DELETE")
#    undef DELETE
#  endif
#endif

#include "opentelemetry/trace/semantic_conventions.h"

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
                                              {{SemanticConventions::RPC_SYSTEM, "grpc"},
                                               {SemanticConventions::RPC_SERVICE, "GreeterService"},
                                               {SemanticConventions::RPC_METHOD, "Greet"},
                                               {SemanticConventions::RPC_GRPC_STATUS_CODE, 0}},
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
