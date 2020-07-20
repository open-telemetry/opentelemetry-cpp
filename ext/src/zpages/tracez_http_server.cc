#pragma once

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <algorithm>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

#include "opentelemetry/ext/zpages/zpages_http_server.h"
#include "opentelemetry/ext/http/server/HttpServer.h"

void runServer() {
  ext::zpages::zPagesHttpServer server("localhost", 32000);
  std::cout << "Hi...\n";
  server.start();
  std::cin.get();
  std::cout << "<ENTER> to stop...\n";
  server.stop();
}

std::thread zPagesRun() {
  return std::thread(runServer);
}

int main(int argc, char* argv[]) {
  zPagesRun().detach();
  std::cout << "Presss <ENTER> to stop...\n";
  std::cin.get();
  std::cout << "Server stopped.\n";
}

