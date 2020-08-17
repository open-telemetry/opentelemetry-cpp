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

  opentelemetry::ext::validation::TraceContextServer server("local host", 5000);

  // Start server
  server.SetClientManager();
  server.start();

  // Wait for console input
  cout << "Presss <ENTER> to stop...\n";
  cin.get();

  // Stop server
  server.stop();
  server.EndClientManager();
  cout << "Server stopped.\n";
  return 0;
}