#include "foo_split.h"

namespace
{
std::vector<std::string> mysplit(std::string s, char delim)
{
  auto propagator = get_propagator();

  gRPCMapCarrier carrier;
  carrier.gRPCMapCarrier::Set("http.header.stub", "temporarily-stubbed");
  auto span  = get_tracer("grpc")->StartSpan("splitfunc");
  auto scope = get_tracer("grpc")->WithActiveSpan(span);

  std::vector<std::string> token_container{};
  std::stringstream ss(s);
  std::string token;

  while (std::getline(ss, token, delim))
    token_container.push_back(token);

  opentelemetry::context::Context ctx1 = opentelemetry::context::Context{"current-span", span};
  opentelemetry::context::Context ctx2 = opentelemetry::context::RuntimeContext::GetCurrent();
  propagator->Inject(carrier, ctx1);
  propagator->Inject(carrier, ctx2);

  span->End();
  return token_container;
}
}  // namespace

std::vector<std::string> split(std::string s, char delim)
{
  auto propagator = get_propagator();

  gRPCMapCarrier carrier;
  carrier.gRPCMapCarrier::Set("http.header.stub", "temporarily-stubbed");

  auto span  = get_tracer("grpc")->StartSpan("splitlib");
  auto scope = get_tracer("grpc")->WithActiveSpan(span);

  opentelemetry::context::Context ctx1 = opentelemetry::context::Context{"current-span", span};
  opentelemetry::context::Context ctx2 = opentelemetry::context::RuntimeContext::GetCurrent();
  propagator->Inject(carrier, ctx1);
  propagator->Inject(carrier, ctx2);

  std::vector<std::string> v = mysplit(s, delim);

  span->End();
  return v;
}
