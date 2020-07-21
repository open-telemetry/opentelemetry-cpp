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

  std::string GetRandomVectorItem(std::vector<std::string> v){
    return v[rand() % v.size()];
  }

  std::string GetDescriptionString() {
    std::string base = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.";
    int start = rand() % 20;
    int end = rand() % (base.length() - start);
    return base.substr(start, end);
  }

  int GetRandomTimeInt() {
    return rand() % 999999 + 10000000000;
  }

  json RunningMockData(int upper = 5, int lower = 0) {
    std::vector<std::string> parent_ids;
    for (int i = 0; i < rand() % 2 + 1; i++) {
      parent_ids.push_back(GetRandomString());
    }
    json temp = json::array();
    for (int i = 0; i < rand() % upper + lower; i++) {
      temp.push_back({
        {"spanid", GetRandomString()},
        {"parentid", GetRandomVectorItem(parent_ids)},
        {"traceid", GetRandomString()},
        {"description", GetDescriptionString()},
        {"start", GetRandomTimeInt()},
      });
    }
    return temp;
  }

  json ErrorMockData(int upper = 5, int lower = 0) {
    std::vector<std::string> error_statuses {"Canceled", "Unknown", "NotFound"};
    std::vector<std::string> parent_ids;
    for (int i = 0; i < rand() % 2 + 1; i++) {
      parent_ids.push_back(GetRandomString());
    }
    auto temp = json::array();
    for (int i = 0; i < rand() % upper + lower; i++) {
      temp.push_back({
        {"spanid", GetRandomString()},
        {"parentid", GetRandomVectorItem(parent_ids)},
        {"traceid", GetRandomString()},
        {"status", GetRandomVectorItem(error_statuses)},
        {"description", GetDescriptionString()},
        {"start", GetRandomTimeInt()},
      });
    }
    return temp;
  }

  json LatencyMockData(int upper = 5, int lower = 0) {
    std::vector<std::string> parent_ids;
    for (int i = 0; i < rand() % 2 + 1; i++) {
      parent_ids.push_back(GetRandomString());
    }
    auto temp = json::array();
    for (int i = 0; i < rand() % upper + lower; i++) {
      auto start_time = GetRandomTimeInt();
      auto end_time = start_time + (rand() % 200 + 1);
      temp.push_back({
        {"spanid", GetRandomString()},
        {"parentid", GetRandomVectorItem(parent_ids)},
        {"traceid", GetRandomString()},
        {"start", start_time},
        {"end", end_time},
        {"description", GetDescriptionString()},
        {"duration", end_time - start_time},
      });
    }
    return temp;
  }

  json CountsMockData(int upper = 5, int lower = 0) {
    auto temp = json::array();
    for (int i = 0; i < rand() % upper + lower; i++) {
      auto latency = json::array({
        rand() % 100,
        rand() % 100,
        rand() % 100,
        rand() % 100,
        rand() % 100,
        rand() % 100,
        rand() % 100,
        rand() % 100,
      });
      temp.push_back({
        {"name", GetRandomString()},
        {"error", rand() % 100},
        {"running", rand() % 100},
        {"latency", latency}
      });
    }
    return temp;
  }

  TracezHandler() {
    data = CountsMockData(30, 10);
  };

  HTTP_SERVER_NS::HttpRequestCallback ServeJsonCb{[&](HTTP_SERVER_NS::HttpRequest const& req,
                                                      HTTP_SERVER_NS::HttpResponse& resp) {
    resp.headers[testing::CONTENT_TYPE] = "application/json";
    if (req.uri == "/tracez/get/aggregations") {
      resp.body = data.dump();
    }
    else if (req.uri.substr(0, 19) == "/tracez/get/running") {
      resp.body = RunningMockData().dump();
    }
    else if (req.uri.substr(0, 17) == "/tracez/get/error") {
      resp.body = ErrorMockData().dump();
    }
    else if (req.uri.substr(0, 19) == "/tracez/get/latency") {
      resp.body = LatencyMockData().dump();
    }
    else {
      resp.body = CountsMockData().dump();
    };
    return 200;
  }};

  std::vector<std::string> GetEndpoints() {
    return endpoints;
  }

 private:
  const std::vector<std::string> endpoints {
    "/tracez/get",
    "/status.json",
  };
  json data;

};

} // namespace zpages
} // namespace ext
