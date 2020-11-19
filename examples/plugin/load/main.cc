#include "opentelemetry/plugin/dynamic_load.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cerr << "Usage: load_plugin <plugin> <config>\n";
    return -1;
  }
  std::string error_message;
  auto factory = opentelemetry::plugin::LoadFactory(argv[1], error_message);
  if (factory == nullptr)
  {
    std::cerr << "Failed to load opentelemetry plugin: " << error_message << "\n";
    return -1;
  }
  std::ifstream config_in{argv[2]};
  if (!config_in.good())
  {
    std::perror("Failed to open config file");
    return -1;
  }
  std::string config{std::istreambuf_iterator<char>{config_in}, std::istreambuf_iterator<char>{}};
  auto tracer = factory->MakeTracer(config, error_message);
  if (tracer == nullptr)
  {
    std::cerr << "Failed to make tracer: " << error_message << "\n";
    return -1;
  }
  auto span = tracer->StartSpan("abc");
  return 0;
}
