// Copyright 2020, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <atomic>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <algorithm>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

// Debug macros
#define HAVE_CONSOLE_LOG
#define HAVE_HTTP_DEBUG

// Macro that allows to implement HTTP server in any namespace
#define HTTP_SERVER_NS testing

namespace HTTP_SERVER_NS
{
};

#include "opentelemetry/ext/http/server/HttpServer.h"

using namespace HTTP_SERVER_NS;
using namespace std;

bool file_get_contents(string filename, std::vector<char>& result)
{
#ifdef _WIN32
    std::replace(filename.begin(), filename.end(), '/', '\\');
#endif
    LOG_INFO("Getting file contents: %s", filename.c_str());
    streampos size;
    ifstream file(filename, ios::in | ios::binary | ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        if (size)
        {
            result.resize(size);
            file.seekg(0, ios::beg);
            file.read(result.data(), size);
        }
        file.close();
        return true;
    }
    return false;
};

string mime_content_type(string filename)
{
    static map<string, string> mimeTypes =
    {
        {"css",  "text/css"},
        {"png",  "image/png"},
        {"js",   "text/javascript"},
        {"htm",  "text/html"},
        {"html", "text/html"},
        {"json", "application/json"},
        {"txt",  "text/plain"},
        {"jpg",  "image/jpeg"},
        {"jpeg", "image/jpeg"},
    };

    std::string ext = filename.substr(filename.find_last_of(".") + 1);
    auto result = mimeTypes[ext];
    if (result == "")
    {
        result = CONTENT_TYPE_TEXT;
    };
    return result;
};

int main(int argc, char* argv[])
{
    atomic<uint32_t> reqCount{0};

    HttpServer server("localhost", 32000);

    // Initialize callback function from lambda
    HttpRequestCallback cb{[&](HttpRequest const& req, HttpResponse& resp) {
        static std::atomic<size_t> baseId{10000};
        if (req.uri == "/status.json")
        {
            resp.headers[CONTENT_TYPE] = "application/json";
            json j = json::array();
            for (size_t i = 0; i < 3; i++)
            {
                baseId++;
                j[i] = {
                    {"type", "span"},
                    {"id", baseId.load()},
                    {"status", "started"}
                };
            }
            resp.body = j.dump();
            LOG_INFO("response: %s", resp.body.c_str());
            return 200;
        };

        // Simple plain text response
        if (req.uri.substr(0, 8) == "/simple/")
        {
            resp.headers[CONTENT_TYPE] = CONTENT_TYPE_TEXT;
            resp.body = "It works!";
            return atoi(req.uri.substr(8).c_str());
        }

        // Echo back the contents of what we received in HTTP POST
        if (req.uri == "/echo/")
        {
            auto it = req.headers.find(CONTENT_TYPE);
            resp.headers[CONTENT_TYPE] = (it != req.headers.end()) ? it->second : CONTENT_TYPE_BIN;
            resp.body = req.content;
            return 200;
        }

        // Simple counter that tracks requests processed
        if (req.uri.substr(0, 7) == "/count/")
        {
            reqCount++;
            resp.headers[CONTENT_TYPE] = CONTENT_TYPE_TEXT;
            resp.message = "200 OK";
            resp.body = "Requests processed: ";
            resp.body += to_string(reqCount);
            return 200;
        }
        return 0;
    }};

    server["/status.json"] = cb;
    server["/simple/"] = cb;
    server["/echo/"] = cb;
    server["/count/"] = cb;

    HttpRequestCallback fileSystemCb{[&](HttpRequest const& req, HttpResponse& resp) {
        auto filename = req.uri.c_str() + 1;
        // Default handler
        std::vector<char> content;
        if (file_get_contents(filename, content))
        {
            resp.headers[CONTENT_TYPE] = mime_content_type(filename);
            resp.body = std::string(content.data(), content.size());
            resp.code = 200;
            resp.message = HttpServer::getDefaultResponseMessage(resp.code);
            return resp.code;
        };

        // Two additional 'special' return codes possible here:
        // 0    - proceed to next handler
        // -1   - immediately terminate and close connection
        resp.headers[CONTENT_TYPE] = CONTENT_TYPE_TEXT;
        resp.code = 404;
        resp.message = HttpServer::getDefaultResponseMessage(resp.code);
        resp.body = resp.message;
        return 404;
    }};
    server["/"] = fileSystemCb;

    // Start server
    server.start();

    // Wait for console input
    cout << "Presss <ENTER> to stop...\n";
    cin.get();

    // Stop server
    server.stop();
    cout << "Server stopped.\n";
}
