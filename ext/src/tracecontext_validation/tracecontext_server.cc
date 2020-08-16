#include "opentelemetry/ext/tracecontext_validation/tracecontext_server.h"

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

  void SetClientManager(HttpClients &http_clients);

  void InitializeCallBack(FileHttpServer &server) { server[test_protocol_] = SendRequestBack; }

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
    char const *begin = content;
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
      while (*ptr && *ptr != ':' && *ptr != ' ' && *ptr != '\r' && *ptr != '\n')
      {
        ptr++;
      }
      if (*ptr != ':')
      {
        return false;
      }
      std::string key1 = Trim(NormalizeName(begin, ptr), '\"');
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
      kv_pairs[key1] = Trim(std::string(begin, ptr), '[', ']');

      ptr++;
      while (*ptr == ' ')
      {
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
      std::string key2 = Trim(NormalizeName(begin, ptr), '\"');
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
      kv_pairs[key2] = Trim(std::string(begin, ptr), '[', ']');
      send_list.push_back(kv_pairs);
    }
    return true;
  }

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
