/*#include "messages.grpc.pb.h"
#include "tracer_common.h"

#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include <chrono>
#include <fstream>
#include <string>
#include <thread>

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/generic/generic_stub.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/support/server_interceptor.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::Status;

using grpc_example::Greeter;
using grpc_example::GreetRequest;
using grpc_example::GreetResponse;

class GreeterInterceptor : public grpc::experimental::Interceptor
{
public:
  GreeterInterceptor(grpc::experimental::ServerRpcInfo *info)
  {
    info_                                        = info;
    const char *method                           = info->method();
    grpc::experimental::ServerRpcInfo::Type type = info->type();
  }

  void Intercept(grpc::experimental::InterceptorBatchMethods *methods) override
  {
    if (methods->QueryInterceptionHookPoint(
            grpc::experimental::InterceptionHookPoints::PRE_SEND_INITIAL_METADATA))
    {
      auto *map = methods->GetSendInitialMetadata();
      std::cout << "PRE_SEND_INITIAL_METADATA SEND_INITIAL_METADATA ELEMENTS: " << std::endl;

      for (const auto &pair : *map)
        std::cout << pair.first << " : " << pair.second << std::endl;
    }

    if (methods->QueryInterceptionHookPoint(
            grpc::experimental::InterceptionHookPoints::PRE_SEND_MESSAGE))
    {
      GreetRequest req;
      auto *buffer       = methods->GetSerializedSendMessage();
      auto copied_buffer = *buffer;
      size_t size        = copied_buffer.Length();
      std::cout << "PRE_SEND_MESSAGE: Byte buffer size: " << size << std::endl;
    }

    if (methods->QueryInterceptionHookPoint(
            grpc::experimental::InterceptionHookPoints::PRE_SEND_STATUS))
    {
      auto *map = methods->GetSendTrailingMetadata();
      std::cout << "PRE_SEND_STATUS SEND_TRAILING_METADATA ELEMENTS: " << std::endl;

      for (const auto &pair : *map)
        std::cout << pair.first << " : " << pair.second << std::endl;
    }
    if (methods->QueryInterceptionHookPoint(
            grpc::experimental::InterceptionHookPoints::POST_RECV_INITIAL_METADATA))
    {
      auto *map = methods->GetRecvInitialMetadata();
      std::cout << "POST_RECV_INITIAL_METADATA RECV_INITIAL_METADATA ELEMENTS: " << std::endl;

      for (const auto &pair : *map)
        std::cout << pair.first << " : " << pair.second << std::endl;
    }
    if (methods->QueryInterceptionHookPoint(
            grpc::experimental::InterceptionHookPoints::POST_RECV_MESSAGE))
    {
      GreetResponse *resp = static_cast<GreetResponse *>(methods->GetRecvMessage());
      std::cout << "Response: " << resp->response() << std::endl;
    }
    if (methods->QueryInterceptionHookPoint(
            grpc::experimental::InterceptionHookPoints::POST_RECV_STATUS))
    {
      auto *map = methods->GetRecvTrailingMetadata();
      std::cout << "POST_RECV_STATUS RECV_TRAILING_METADATA ELEMENTS: " << std::endl;

      for (const auto &pair : *map)
        std::cout << pair.first << " : " << pair.second << std::endl;
    }
    if (methods->QueryInterceptionHookPoint(
            grpc::experimental::InterceptionHookPoints::POST_RECV_CLOSE))
    {}
    methods->Proceed();
  }

private:
  grpc::experimental::ServerRpcInfo *info_;
};

class GreeterInterceptorFactory : public grpc::experimental::ServerInterceptorFactoryInterface
{
public:
  virtual grpc::experimental::Interceptor *CreateServerInterceptor(
      grpc::experimental::ServerRpcInfo *info) override
  {
    return new GreeterInterceptor(info);
  }
};

class GreeterServer final : public Greeter::Service
{
public:
  Status Greet(ServerContext *context,
               const GreetRequest *request,
               GreetResponse *response) override
  {
    opentelemetry::trace::StartSpanOptions options;
    options.kind               = opentelemetry::trace::SpanKind::kServer;
    std::string peer_name      = context->peer();
    std::string peer_name_copy = peer_name;
    std::string delim          = ":";
    std::vector<std::string> peer_name_attributes{};
    std::cout << "peer name: " << peer_name << std::endl;
    ;
    size_t idx;
    while ((idx = peer_name_copy.find(delim)) != std::string::npos)
    {
      peer_name_attributes.push_back(peer_name_copy.substr(0, idx));
      peer_name_copy.erase(0, idx + delim.length());
    }
    for (std::string s : peer_name_attributes)
    {
      std::cout << s << std::endl;
    }

    std::string span_name = "grpc server";
    auto span             = get_tracer("grpc-server")
                    ->StartSpan(span_name,
                                {{"rpc.system", "grpc"},
                                 {"rpc.service", "grpc-example.GreetService"},
                                 {"rpc.method", "Greet"},
                                 {"net.peer.ip", peer_name_attributes.at(1)},
                                 {"net.peer.port", peer_name_attributes.at(2)},
                                 {"rpc.grpc.status_code", "0"}},
                                options);
    auto scope = get_tracer("grpc-server")->WithActiveSpan(span);

    std::string req = request->request();
    std::cout << "grpc_client says: " << req << std::endl;
    std::string message = "The pleasure is mine.";
    response->set_response(message);

    span->End();
    return Status::OK;
  }
};

void RunServer(uint16_t port)
{
  std::string address("0.0.0.0:" + std::to_string(port));
  GreeterServer service;
  ServerBuilder builder;

  builder.RegisterService(&service);
  builder.AddListeningPort(address, grpc::InsecureServerCredentials());
  std::vector<std::unique_ptr<grpc::experimental::ServerInterceptorFactoryInterface>> creators;
  creators.reserve(1);
  creators.push_back(std::unique_ptr<GreeterInterceptorFactory>(new GreeterInterceptorFactory()));
  builder.experimental().SetInterceptorCreators(std::move(creators));

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on port: " << address << std::endl;
  server->Wait();
  server->Shutdown();
}

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
*/