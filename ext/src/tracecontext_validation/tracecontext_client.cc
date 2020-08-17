#include "opentelemetry/ext/tracecontext_validation/tracecontext_client.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace validation
{
bool HttpClients::HttpClient::SendRequest(std::string url)
{
  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fields);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, -1);
  }
  res = curl_easy_perform(curl);
  if (res != CURLE_OK)
    return false;
  else
    return true;
}

// Set the headers of the post request to be transmitted.
void HttpClients::HttpClient::SetHeaders(std::vector<std::string> headers)
{
  for (std::vector<std::string>::iterator it = headers.begin(); it != headers.end(); it++)
  {
    list = curl_slist_append(list, (*it).c_str());
  }
}

// Set the POST fields of the post request to be transmitted, need to percent-encode '=' sign
// in trace state to "%3D"
void HttpClients::HttpClient::AddPostField(std::string post_field_name, std::string post_field_value)
{
  char *name  = curl_easy_escape(curl, post_field_name.c_str(), 0);
  char *value = curl_easy_escape(curl, post_field_value.c_str(), 0);
  fields += "&" + std::string(name) + "=" + std::string(value);
}

HttpClients::HttpClient HttpClients::StartNewClient() { return HttpClients::HttpClient(); }
} // namespace validation
} // namespace ext
OPENTELEMETRY_END_NAMESPACE
