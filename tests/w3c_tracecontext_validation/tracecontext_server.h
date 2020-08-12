#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "opentelemetry/ext/http/server/http_server.h"
#include "opentelemetry/tests/w3c_tracecontext_validation/tracecontext_client.h"

namespace HTTP_SERVER_NS
{

class TraceContextServer : public HTTP_SERVER_NS::HttpServer
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

  void SetClientManager(HttpClients &http_clients) {
     clients = http_clients;
  }

  /**
   * Set the HTTP server to serve static files from the root of host:port.
   * Derived HTTP servers should initialize the file endpoint AFTER they
   * initialize their own, otherwise everything will be served like a file
   * @param server should be an instance of this object
   */
  void InitializeFileEndpoint(FileHttpServer &server) { server[test_protocol_] = SendRequestBack; }

private:
  static std::string NormalizeName(char const *begin, char const *end)
  {
     std::string result(begin, end);
     bool first = true;
     for (char &ch : result)
     {
        if (first)
        {
          ch    = static_cast<char>(::toupper(ch));
          first = false;
        }
        else if (ch == '-')
        {
          first = true;
        }
        else
        {
          ch = static_cast<char>(::tolower(ch));
        }
     }
     return result;
  }

  std::string Trim(std::string& str)
  {
      std::size_t first = str.find_first_not_of('\"');
      if (std::string::npos == first)
      {
          return str;
      }
      std::size_t last = str.find_last_not_of('\"');
      return str.substr(first, (last - first + 1));
  }

  bool ParseBody(Connection &conn)
  {
     char const *begin = conn.receiveBuffer.c_str();
     char const *ptr   = begin;
     while (*ptr == ' ')
     {
        ptr++;
     }
     if (*ptr == '[')
     {
        ptr++;
     }
     // Key-Value Pairs
     while (*ptr != '\r' && *ptr != '\n')
     {
        // Begin
        if (*ptr == '{') {
           ptr++;
        }
        // Key1
        begin = ptr;
        while (*ptr && *ptr != ':' && *ptr != ' ' && *ptr != '\r' && *ptr != '\n')
        {
          ptr++;
        }
        if (*ptr != ':')
        {
          return false;
        }
        std::string key1 = Trim(NormalizeName(begin, ptr));
        ptr++;
        while (*ptr == ' ')
        {
          ptr++;
        }
        // Value1
        begin = ptr;
        while (*ptr && *ptr != '\r' && *ptr != '\n' && *ptr != ',')
        {
          ptr++;
        }
        conn.request.headers[key1] = Trim(std::string(begin, ptr));

        while (*ptr == ' ') {
          ptr++;
        }
        // Key2
        begin = ptr;
        while (*ptr && *ptr != ':' && *ptr != ' ' && *ptr != '\r' && *ptr != '\n')
        {
          ptr++;
        }
        if (*ptr != ':')
        {
          return false;
        }
        std::string key2 = Trim(NormalizeName(begin, ptr));
        ptr++;
        while (*ptr == ' ')
        {
          ptr++;
        }
        // Value2
        begin = ptr;
        while (*ptr && *ptr != '\r' && *ptr != '\n' && *ptr != '}')
        {
          ptr++;
        }
        conn.request.headers[key2] = Trim(std::string(begin, ptr));
        if (*ptr == '\r')
        {
          ptr++;
        }
        if (*ptr != '\n')
        {
          return false;
        }
        ptr++;
     }
     if (*ptr == '\r')
     {
        ptr++;
     }
     if (*ptr != '\n')
     {
        return false;
     }
     ptr++;
     return true;
  }

  /**
   * Sets the response object with the correct file data based on the requested
   * file address, or return 404 error if a file isn't found
   * @param req is the HTTP request, which we use to figure out the response to
   * send
   * @param resp is the HTTP response we want to send to the frontend, including
   * file data
   */
  HTTP_SERVER_NS::HttpRequestCallback SendRequestBack{
     [&](HTTP_SERVER_NS::HttpRequest const &req, HTTP_SERVER_NS::HttpResponse &resp) {
         HttpClient client = clients.StartNewClient();
         return 404;
     }};

  const std::string test_protocol_ = "/test/";
  HttpClients clients;
};

}  // namespace HTTP_SERVER_NS
