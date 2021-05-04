// #define HAVE_CONSOLE_LOG

#include <cstdio>
#include <cstdlib>
#include <list>
#include <map>
#include <set>
#include <string>
#include <thread>

#include <gtest/gtest.h>

#include <opentelemetry/ext/http/server/socket_server.h>

using namespace SOCKET_SERVER_NS;
using namespace std;

namespace testing
{
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
        char* tmp = getenv("TMPDIR");
        if (tmp != NULL)
        {
            result = tmp;
        }
        else
        {
#ifdef P_tmpdir
            if (P_tmpdir != NULL)
                result = P_tmpdir;
#endif
        }
#ifdef _PATH_TMP
        if (result.empty())
        {
            result = _PATH_TMP;
        }
#endif
        if (result.empty())
        {
            result = "/tmp";
        }
        result += "/";
#endif
        return result;
    }

    void StartEchoServer(SocketServer& server)
    {
        server.onRequest = [&](SocketServer::Connection& conn) {
            // std::cout << "SocketServer::onRequest: " << conn.received_buffer << std::endl;
            conn.buffer_sent.clear();
            conn.buffer_received.swap(conn.buffer_sent);
            // Signal to Reactor that it's time to respond
            conn.state.insert(SocketServer::Connection::Responding);
        };

        server.onResponse = [&](SocketServer::Connection& conn) {
            // std::cout << "SocketServer::onResponse: " << conn.sendBuffer << std::endl;
        };

        server.Start();
        // Client must match the server params.
        Socket client(server.server_socket_params);
        client.connect(server.address());

        // ping ->
        std::string requestText("Hello!");
        // std::cout << "ping: ";
        // std::cout << requestText << std::endl;
        int res = client.send(requestText.c_str(), requestText.length());
        EXPECT_EQ(res, requestText.size());

        // <- pong
        char responseText[1024] = {0};
        client.recv(responseText, sizeof(responseText));
        // std::cout << "pong: ";
        // std::cout << (const char*)(responseText) << std::endl;
        EXPECT_TRUE(requestText == responseText);
        client.close();
        server.Stop();
    }

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
        StartEchoServer(server);
    }

    TEST(SocketTests, BasicUdpEchoTest)
    {
        SocketParams params{AF_INET, SOCK_DGRAM, 0};
        SocketAddr destination("127.0.0.1:4000");
        SocketServer server(destination, params);
        StartEchoServer(server);
    }

    TEST(SocketTests, BasicUnixDomainEchoTest)
    {
        auto socket_name = GetTempDirectory();
        SocketParams params{AF_UNIX, SOCK_STREAM, 0};
        // Store messenger.sock named Unix domain socket in temp dir
        socket_name += "messenger.sock";
        // cpp/io/c/remove
        std::cout << "Temporary AF_UNIX socket name: " << socket_name << std::endl;
        std::remove(socket_name.c_str());
        SocketAddr destination(socket_name.c_str(), true);
        SocketServer server(destination, params);
        StartEchoServer(server);
    }
}