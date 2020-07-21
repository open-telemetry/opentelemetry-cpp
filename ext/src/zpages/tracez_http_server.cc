#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <chrono>

#include "opentelemetry/ext/zpages/zpages_http_server.h"
#include "opentelemetry/ext/http/server/HttpServer.h"

void runServer() {
  ext::zpages::zPagesHttpServer server("localhost", 32000);
  server.start();
  // Keeps zPages server up indefinitely
  while (1) std::this_thread::sleep_for(std::chrono::hours(10));
  server.stop();
}

std::thread zPagesRun() {
  return std::thread(runServer);
}

int main(int argc, char* argv[]) {
  zPagesRun().detach();
  std::cout << "Presss <ENTER> to stop...\n";
  std::cin.get();
}

