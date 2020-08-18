#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "opentelemetry/version.h"
#include "opentelemetry/ext/http/server/http_server.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/trace/default_span.h"
#include "opentelemetry/trace/propagation/http_trace_context.h"
#include "opentelemetry/ext/tracecontext_validation/tracecontext_client.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace validation
{
class TraceContextServer : public HTTP_SERVER_NS::HttpServer
{
public:
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
    InitializeCallBack(*this);
    format = trace::propagation::HttpTraceContext<std::map<std::string, std::string>>();
  };

  void SetClientManager() { clients = nostd::unique_ptr<HttpClients>(new HttpClients()); }

  void EndClientManager() { clients.get()->~HttpClients(); }

  /**
   * Set the HTTP server to serve static files from the root of host:port.
   * Derived HTTP servers should initialize the file endpoint AFTER they
   * initialize their own, otherwise everything will be served like a file
   * @param server should be an instance of this object
   */
  void InitializeCallBack(TraceContextServer &server) { server[test_protocol_] = SendRequestBack; }

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

  std::string Trim(std::string &str, char delimiter) { return Trim(str, delimiter, delimiter); }

  std::string Trim(std::string &str, char delimiter_l, char delimiter_r)
  {
    std::size_t first = str.find_first_not_of(delimiter_l);
    if (std::string::npos == first)
    {
      return str;
    }
    std::size_t last = str.find_last_not_of(delimiter_r);
    return str.substr(first, (last - first + 1));
  }

  bool ParseBody(std::string content, std::vector<std::map<std::string, std::string>> &send_list)
  {
    std::cout<<content<<std::endl;
    char const *begin = content.c_str();
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
      std::map<std::string, std::string> kv_pairs;
      // Begin
      if (*ptr == ',')
      {
        ptr++;
      }
      while (*ptr == ' ')
      {
        ptr++;
      }
      if (*ptr == '{')
      {
        ptr++;
      }
      // Key1
      begin = ptr;
      while (*ptr && *ptr != ':' && *ptr != '\r' && *ptr != '\n')
      {
        ptr++;
      }
      if (*ptr != ':')
      {
        std::cout<<"It is not a colon but a "<<*ptr<<std::endl;
        return false;
      }
      std::string key1 = NormalizeName(begin, ptr);
      key1 = Trim(key1, '\"');
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
      std::string val1 = std::string(begin, ptr);
//      val1 = Trim(val1,'[', ']');
      kv_pairs[key1] = Trim(val1, '\"', '\"');
      std::cout<<"first k-v pairs get"<<std::endl;
      ptr++;
      while (*ptr == ' ')
      {
        ptr++;
      }
      // Key2
      begin = ptr;
      while (*ptr && *ptr != ':' && *ptr != '\r' && *ptr != '\n')
      {
        ptr++;
      }
      if (*ptr != ':')
      {
        std::cout<<"It is not a colon but a "<<*ptr<<std::endl;
        return false;
      }
      std::string key2 = NormalizeName(begin, ptr);
      key2 = Trim(key2, '\"');
      std::cout<<"key 2 is"<<key2<<std::endl;
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
      std::string val2 = std::string(begin, ptr);
//      val2 = Trim(val1,'[', ']');
      std::cout<<"value 2 is"<<val2<<std::endl;
      kv_pairs[key2] = Trim(val2, '\"', '\"');
      send_list.push_back(kv_pairs);
      if (*ptr == '}') {
        ptr++;
      }
    }
    return true;
  }

  bool FormHeader(struct curl_slist *chunk, std::map<std::string,std::string> headers) {
    for (std::map<std::string,std::string>::iterator it = headers.begin(); it != headers.end(); it++) {
        std::string item = (it->first) + ":" + (it->second);
        chunk = curl_slist_append(chunk, item.c_str());
    }
    return true;
  }

  static bool pull_one_url(std::string url, std::string value, context::Context context)
  {
    CURL *curl = curl_easy_init();
    if (curl == nullptr) {
        std::cout<<"invalid curl pointer initialized"<<std::endl;
    }
    struct curl_slist *chunk = NULL;
    std::map<std::string, std::string> carrier = {};
    format.Inject(Setter, carrier, ctx2);

    FormHeader(chunk, carrier);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, value.c_str());

    std::cout<<"performing"<<std::endl;
    CURLcode res = curl_easy_perform(curl); /* ignores error */
    std::cout<<"clean up"<<std::endl;
    curl_easy_cleanup(curl);
    curl_slist_free_all(chunk);
    if (res == CURLE_OK) {
        std::cout<<"message of url "<<url<<" delivered"<<std::endl;
        return true;
    } else {
        std::cout<<"message of url "<<url<<" not delivered, code "<<res<<std::endl;
        return false;
    }
  }

  static nostd::string_view Getter(const std::map<std::string, std::string> &carrier,
                                   nostd::string_view trace_type = "traceparent")
  {
    auto it = carrier.find(std::string(trace_type));
    if (it != carrier.end())
    {
      return nostd::string_view(it->second);
    }
    return "";
  }

  static void Setter(std::map<std::string, std::string> &carrier,
                     nostd::string_view trace_type        = "traceparent",
                     nostd::string_view trace_description = "")
  {
    carrier[std::string(trace_type)] = std::string(trace_description);
  }

  HTTP_SERVER_NS::HttpRequestCallback SendRequestBack{
      [&](HTTP_SERVER_NS::HttpRequest const &req, HTTP_SERVER_NS::HttpResponse &resp) {
        std::vector<std::map<std::string, std::string>> send_list;
        ParseBody(req.content.c_str(), send_list);
        context::Context ctx1 =
            context::Context("current-span", nostd::shared_ptr<trace::Span>(new trace::DefaultSpan()));
        context::Context ctx2 = format.Extract(Getter, req.headers, ctx1);
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
            if (!pull_one_url(url, arguments, ctx2)) {
                return 404;
            }
          }
          else
          {
            return 404;
          }
        }
        return 200;
      }};

  const std::string test_protocol_ = "/test";
  const int kMaxUrlPerTest = 32;
  nostd::unique_ptr<HttpClients> clients;
  trace::propagation::HttpTraceContext<std::map<std::string, std::string>> format;
};
}// namespace validation
}// namespace ext
OPENTELEMETRY_END_NAMESPACE

