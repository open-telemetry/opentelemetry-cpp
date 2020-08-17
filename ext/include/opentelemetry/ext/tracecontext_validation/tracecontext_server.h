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
  struct ArgStruct {
      std::string url;
      std::string name;
      std::string value;
  };

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
      val1 = Trim(val1,'[', ']');
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
      val2 = Trim(val1,'[', ']');
      std::cout<<"value 2 is"<<val2<<std::endl;
      kv_pairs[key2] = Trim(val2, '\"', '\"');
      send_list.push_back(kv_pairs);
      if (*ptr == '}') {
        ptr++;
      }
    }
    return true;
  }

  static void *pull_one_url(void * args)
  {
    struct ArgStruct *arguments = (struct ArgStruct *)args;
    std::cout<<"pull 1"<<std::endl;
    CURL *curl;
    CURLcode res;
    char *name  = curl_easy_escape(curl, arguments->name.c_str(), 0);
    char *value = curl_easy_escape(curl, arguments->value.c_str(), 0);
    std::string fields = std::string(name) + "=" + std::string(value);

    std::cout<<"pull 2 - url is: "<<(arguments->url)<<std::endl;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, arguments->url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fields.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1);
    std::cout<<"pull 3"<<std::endl;
    res = curl_easy_perform(curl); /* ignores error */
    if (res == CURLE_OK || res == 0) {
        std::cout<<"message of url "<<(arguments->url)<<" delivered"<<std::endl;
    } else {
        std::cout<<"message of url "<<(arguments->url)<<" not delivered, code "<<res<<std::endl;
    }
    curl_easy_cleanup(curl);
    std::cout<<"pull 4"<<std::endl;

    return NULL;
  }

  HTTP_SERVER_NS::HttpRequestCallback SendRequestBack{
      [&](HTTP_SERVER_NS::HttpRequest const &req, HTTP_SERVER_NS::HttpResponse &resp) {
        pthread_t tid[kMaxUrlPerTest];
        int count = 0;
        std::vector<std::map<std::string, std::string>> send_list;
        ParseBody(req.content.c_str(), send_list);
        std::cout<<"send list size: "<<send_list.size()<<std::endl;
        for (std::map<std::string, std::string> kv_pairs : send_list)
        {
          std::string url       = "";
          std::string arguments = "";
          for (std::map<std::string, std::string>::iterator it = kv_pairs.begin();
               it != kv_pairs.end(); it++)
          {
            std::cout<<"extracted k-v: "<<it->first<<" "<<it->second<<" value length "<<it->second.length()<<std::endl;
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
//            HttpClients::HttpClient client = clients.get()->StartNewClient();
//            std::cout<<"sending to url-2"<<std::endl;
//            client.AddPostField("arguments", arguments);
//            std::cout<<"sending to url-3"<<std::endl;
//            client.SendRequest(url);
//            std::cout<<"sending to url-4"<<std::endl;
//            client.~HttpClient();
//            std::cout<<"sending to url-5"<<std::endl;
            struct ArgStruct *args = (struct ArgStruct *)malloc(10000);
            std::cout<<"sendingto url "<<url<<" arguments "<<arguments<<std::endl;
            args->url = url;
            args->name = "arguments";
            args->value = arguments;
            int error = pthread_create(&tid[count],
                                       NULL, /* default attributes please */
                                       pull_one_url,
                                       args);
            if(0 != error)
                std::cout<<"sending fails"<<std::endl;
//              fprintf(stderr, "Couldn't run thread number %d, errno %d\n", count, error);
            else
                std::cout<<"sending succeeds"<<std::endl;
//              fprintf(stderr, "Thread %d, gets %s\n", count, url);
            free(args);
            count++;
          }
          else
          {
            return 404;
          }
        }
        for(int i = 0; i< count; i++) {
          pthread_join(tid[i], NULL);
          std::cout<<"Thread "<<i<<" terminated"<<std::endl;
        }
        return 200;
      }};

  const std::string test_protocol_ = "/test";
  const int kMaxUrlPerTest = 32;
  nostd::unique_ptr<HttpClients> clients;
};
}// namespace validation
}// namespace ext
OPENTELEMETRY_END_NAMESPACE

