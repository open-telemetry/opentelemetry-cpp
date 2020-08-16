#include "opentelemetry/ext/tracecontext_validation/tracecontext_client.h"
#include "opentelemetry/ext/tracecontext_validation/tracecontext_server.h"

#include <algorithm>
#include <atomic>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include "curl/curl.h"

using namespace std;
int main()
{
  atomic<uint32_t> reqCount{0};

  opentelemetry::ext::validation::TraceContextServer server("localhost", 5000);
  opentelemetry::ext::validation::HttpClients clients = opentelemetry::ext::validation::HttpClients(); // Initialize libcurl

  // Start server
  server.start();

  // Wait for console input
  cout << "Presss <ENTER> to stop...\n";
  cin.get();

  // Stop server
  server.stop();
  cout << "Server stopped.\n";
  clients.~HttpClients();
  return 0;
}