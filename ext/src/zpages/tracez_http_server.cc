#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <memory>

#include "opentelemetry/ext/zpages/tracez_processor.h"
#include "opentelemetry/ext/zpages/tracez_data_aggregator.h"
#include "opentelemetry/sdk/trace/recordable.h"
#include "opentelemetry/sdk/trace/tracer.h"
#include "opentelemetry/ext/zpages/zpages_http_server.h"
#include "opentelemetry/ext/http/server/HttpServer.h"

using namespace opentelemetry::sdk::trace;
using namespace opentelemetry::ext::zpages;


void runServer(std::shared_ptr<TracezSpanProcessor>& processor) {
  ext::zpages::zPagesHttpServer server("localhost", processor,32000);
  server.start();
  // Keeps zPages server up indefinitely
  while (1) std::this_thread::sleep_for(std::chrono::hours(10));
  server.stop();
}

std::thread zPagesRun(std::shared_ptr<TracezSpanProcessor>& processor) {
  return std::thread(runServer,std::ref(processor));
}

int main(int argc, char* argv[]) {
  std::unique_ptr<TracezDataAggregator> tracez_data_aggregator;
  std::shared_ptr<opentelemetry::trace::Tracer> tracer;
  std::shared_ptr<TracezSpanProcessor> processor(new TracezSpanProcessor());
  tracer =
      std::shared_ptr<opentelemetry::trace::Tracer>(new Tracer(processor));
      
  zPagesRun(processor).detach();
  std::cout << "Presss <ENTER> to stop...\n";
  std::cin.get();
}

