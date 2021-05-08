#include <sstream>
#include <vector>
#include "../tracer_common.h"
#include "grpc_map_carrier.h"
#include "opentelemetry/trace/provider.h"
#include "foo_split.h"

namespace
{
std::vector<std::string> mysplit(std::string s, char delim)
{
  auto propagator = get_propagator();

  gRPCMapCarrier carrier;
  carrier.gRPCMapCarrier::Set("http.header.stub", "temporarily-stubbed");
  auto span  = get_tracer("grpc-lib")->StartSpan("splitfunc");
  auto scope = get_tracer("grpc-lib")->WithActiveSpan(span);

  std::vector<std::string> token_container{};
  std::stringstream ss(s);
  std::string token;

  while (std::getline(ss, token, delim))
    token_container.push_back(token);

  opentelemetry::context::Context ctx1 = opentelemetry::context::Context{"current-span", span};
  opentelemetry::context::Context ctx2 = propagator->Extract(carrier, ctx1);
  gRPCMapCarrier carrier2;
  propagator->Inject(carrier2, ctx2);

  span->End();
  return token_container;
}
}  // namespace

std::vector<std::string> split(std::string s, char delim)
{
  auto propagator = get_propagator();

  gRPCMapCarrier carrier;
  carrier.gRPCMapCarrier::Set("http.header.stub", "temporarily-stubbed");

  auto span                            = get_tracer("grpc-lib")->StartSpan("splitlib");
  auto scope                           = get_tracer("grpc-lib")->WithActiveSpan(span);
  opentelemetry::context::Context ctx1 = opentelemetry::context::Context{"current-span", span};
  opentelemetry::context::Context ctx2 = propagator->Extract(carrier, ctx1);
  gRPCMapCarrier carrier2;
  propagator->Inject(carrier2, ctx2);

  std::vector<std::string> v = mysplit(s, delim);

  span->End();
  return v;
}
