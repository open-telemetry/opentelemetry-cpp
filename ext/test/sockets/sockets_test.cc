// Uncomment this line for additional debugging:
// #define HAVE_CONSOLE_LOG

#include <cstdio>
#include <cstdlib>
#include <list>
#include <map>
#include <set>
#include <string>
#include <thread>

#include <gtest/gtest.h>

#include <opentelemetry/ext/net/common/socket_server.h>

using namespace SOCKET_SERVER_NS;
using namespace std;

namespace testing
{
static const int kMaxConnections = 16;

/**
 * @brief Obtain path to temporary directory
 * @return Temporary Directory
 */
static inline std::string GetTempDirectory()
{
  std::string result = "";
#ifdef _WIN32
  char temp_path_buffer[MAX_PATH + 1] = {0};
  ::GetTempPathA(MAX_PATH, temp_path_buffer);
  result = temp_path_buffer;
#else  // Unix
  char *tmp = getenv("TMPDIR");
  if (tmp != NULL)
  {
    result = tmp;
  }
  else
  {
#  ifdef P_tmpdir
    if (P_tmpdir != NULL)
      result = P_tmpdir;
#  endif
  }
#  ifdef _PATH_TMP
  if (result.empty())
  {
    result = _PATH_TMP;
  }
#  endif
  if (result.empty())
  {
    result = "/tmp";
  }
  result += "/";
#endif
  return result;
}

std::string GenerateBigString(size_t maxLength = 60000)
{
  char *bigBuff = (char *)calloc(maxLength, sizeof(char));
  for (size_t i = 0; i < sizeof(bigBuff); i++)
  {
    bigBuff[i] = char(i % 255);
  }
  bigBuff[maxLength] = 0;
  std::string bigString((const char *)bigBuff, maxLength);
  free(bigBuff);
  return bigString;
}

struct EchoServerTest
{
  SocketServer &server;

  EchoServerTest(SocketServer &server) : server(server)
  {
    server.onRequest = [&](SocketServer::Connection &conn) {
      conn.response_buffer.clear();
      conn.request_buffer.swap(conn.response_buffer);
      // Signal to Reactor that it's time to respond
      conn.state.insert(SocketServer::Connection::Responding);
    };

    server.onResponse = [&](SocketServer::Connection &conn) {
    };
  }

  void Start() { server.Start(); }

  void Stop() { server.Stop(); }

  void PingPong(std::string request_text, size_t numIterations = 1)
  {
    for (size_t i = 0; i < numIterations; i++)
    {
      Socket client(server.server_socket_params);
      client.connect(server.address());

      // -> ping
      int total_bytes_sent = client.send(request_text.c_str(), request_text.length());
      EXPECT_EQ(total_bytes_sent, request_text.size());

      // <- pong
      std::string response_text;
      response_text.resize(request_text.size(), 0);
      size_t total_bytes_received = client.readall(response_text);
      EXPECT_EQ(total_bytes_received, total_bytes_sent);
      EXPECT_EQ(request_text, response_text);
      client.close();
    }
  }
};

TEST(SocketTests, IPv4_SocketAddr_toString)
{
  std::string host_port_ipv4("127.0.0.1:3000");
  SocketAddr destination(host_port_ipv4.c_str());
  EXPECT_TRUE(host_port_ipv4 == destination.toString());
}

TEST(SocketTests, IPv6_SocketAddr_toString)
{
  std::string host_port_ipv6("[fe80::c018:4a9b:3681:4e41]:3000");
  SocketAddr destination(host_port_ipv6.c_str());
  EXPECT_TRUE(host_port_ipv6 == destination.toString());
}

TEST(SocketTests, BasicTcpEchoTest)
{
  SocketParams params{AF_INET, SOCK_STREAM, 0};
  SocketAddr destination("127.0.0.1:3000");
  SocketServer server(destination, params);
  EchoServerTest test(server);
  test.Start();
  test.PingPong("Hello, world!");
  test.Stop();
}

TEST(SocketTests, ManyPacketsTcpEchoTest)
{
  SocketParams params{AF_INET, SOCK_STREAM, 0};
  SocketAddr destination("127.0.0.1:3000");
  SocketServer server(destination, params);
  EchoServerTest test(server);
  test.Start();
  // Note that our test server can't listen
  // on more than 64 concurrent connections
  test.PingPong("Hello, world!", kMaxConnections);
  test.Stop();
}

TEST(SocketTests, BasicUdpEchoTest)
{
  SocketParams params{AF_INET, SOCK_DGRAM, 0};
  SocketAddr destination("127.0.0.1:4000");
  SocketServer server(destination, params);
  EchoServerTest test(server);
  test.Start();
  test.PingPong("Hello, world!");
  test.Stop();
}

TEST(SocketTests, BasicUnixDomainEchoTest)
{
  auto socket_name = GetTempDirectory();
  SocketParams params{AF_UNIX, SOCK_STREAM, 0};
  // Store messenger.sock named Unix domain socket in temp dir
  socket_name += "messenger.sock";
  // cpp/io/c/remove
  LOG_TRACE("Temporary AF_UNIX socket name=%s", socket_name.c_str());
  std::remove(socket_name.c_str());
  SocketAddr destination(socket_name.c_str(), true);
  SocketServer server(destination, params);
  EchoServerTest test(server);
  test.Start();
  test.PingPong("Hello, world!");
  test.Stop();
}

TEST(SocketTests, ManyPacketsUnixDomainEchoTest)
{
  auto socket_name = GetTempDirectory();
  SocketParams params{AF_UNIX, SOCK_STREAM, 0};
  // Store messenger.sock named Unix domain socket in temp dir
  socket_name += "messenger.sock";
  // cpp/io/c/remove
  LOG_TRACE("Temporary AF_UNIX socket name=%s", socket_name.c_str());
  std::remove(socket_name.c_str());
  SocketAddr destination(socket_name.c_str(), true);
  SocketServer server(destination, params);
  EchoServerTest test(server);
  test.Start();
  test.PingPong("Hello, world!", kMaxConnections);
  test.Stop();
}

}  // namespace testing