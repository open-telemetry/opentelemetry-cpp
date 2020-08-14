#pragma once

#include "tracecontext_client.h"
#include "tracecontext_server.h"

#include <atomic>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <algorithm>
#include <curl/curl.h>

namespace HTTP_SERVER_NS {

int main() {
    atomic<uint32_t> reqCount{0};

    HttpServer server("localhost", 5000);
    // curl_global_init(CURL_GLOBAL_ALL);
    //HttpClients clients = HttpClients(); // Initialize libcurl

    HttpRequestCallback httpCallback{[&](HttpRequest const& req, HttpResponse& resp) {
        return 200;
    }};
    server["/test"] = httpCallback;

    // Start server
    server.start();

    // Wait for console input
    cout << "Presss <ENTER> to stop...\n";
    cin.get();

    // Stop server
    server.stop();
    cout << "Server stopped.\n";
    //clients.~HttpClients();
}

}