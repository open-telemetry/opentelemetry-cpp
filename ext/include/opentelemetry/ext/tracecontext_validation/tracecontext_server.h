#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "opentelemetry/ext/http/server/http_server.h"
#include "opentelemetry/ext/tracecontext_validation/tracecontext_client.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace HTTP_SERVER_NS
{

class TraceContextServer : public HttpServer
{
protected:
  /**
   * Construct the server by initializing the endpoint for serving static files,
   * which show up on the web if the user is on the given host:port. Static
   * files can be seen relative to the folder where the executable was ran.
   */
  TraceContextServer(const std::string &host = "127.0.0.1", int port = 3333) : HttpServer()
  {
    std::ostringstream os;
    os << host << ":" << port;
    setServerName(os.str());
    addListeningPort(port);
  };

  void SetClientManager(HttpClients &http_clients);

  /**
   * Set the HTTP server to serve static files from the root of host:port.
   * Derived HTTP servers should initialize the file endpoint AFTER they
   * initialize their own, otherwise everything will be served like a file
   * @param server should be an instance of this object
   */
  void InitializeFileEndpoint(FileHttpServer &server);

private:
  static std::string NormalizeName(char const *begin, char const *end);

  std::string Trim(std::string &str, char delimiter);

  std::string Trim(std::string &str, char delimiter_l, char delimiter_r);

  bool ParseBody(std::string content, std::vector<std::map<std::string, std::string>> &send_list);

  HTTP_SERVER_NS::HttpRequestCallback SendRequestBack{
      [&](HTTP_SERVER_NS::HttpRequest const &req, HTTP_SERVER_NS::HttpResponse &resp) {
        std::vector<std::map<std::string, std::string>> send_list;
        ParseBody(req.content.c_str(), send_list);
        for (std::map<std::string, std::string> kv_pairs : send_list)
        {
          std::string url       = "";
          std::string arguments = "";
          for (std::map<std::string, std::string>::iterator it = kv_pairs.begin();
               it != kv_pairs.end(); it++)
          {
            if (it->first == "url")
            {
              url = it->second;
            }
            else if (it->first == "arguments")
            {
              arguments = it->second;
            }
          }
          if (url != "")
          {
            HttpClient client = clients.StartNewClient();
            client.AddPostField("arguments", arguments);
            client.SendRequest(url);
            client.~HttpClient();
          }
          else
          {
            return 404;
          }
        }
        return 200;
      }};

  const std::string test_protocol_ = "/test/";
  HttpClients clients;
};
}  // namespace HTTP_SERVER_NS
OPENTELEMETRY_END_NAMESPACE

