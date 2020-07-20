#pragma once

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

#include "nlohmann/json.hpp"
#include "opentelemetry/ext/zpages/zpages_http_server.h"
using json = nlohmann::json;

namespace ext
{
namespace zpages
{


class TracezHandler {
 public:

  std::string GetRandomString() {
    std::string s = "";
    const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < rand() % 8 + 2; ++i) {
        s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return s;
  }

  json GeneralMockData(int upper = 5, int lower = 0) {
    json temp = json::array();
    for (int i = 0; i < rand() % upper + lower; i++) {
      temp [i] = {
        {"name", GetRandomString()},
        {"error", rand() % 100},
        {"running", rand() % 100},
      };
    }
    return temp;
  }

  TracezHandler() {};

  HTTP_SERVER_NS::HttpRequestCallback ServeJsonCb{[&](HTTP_SERVER_NS::HttpRequest const& req,
                                                      HTTP_SERVER_NS::HttpResponse& resp) {
    resp.headers[testing::CONTENT_TYPE] = "application/json";
    if (req.uri == "/status.json") {
      resp.body = GeneralMockData().dump();
    };
    return 200;
  }};

  std::vector<std::string> GetEndpoints() {
    return endpoints;
  }

 private:
  const std::vector<std::string> endpoints {
    "/get/tracez",
    "/status.json",
  };

};

} // namespace zpages
} // namespace ext
