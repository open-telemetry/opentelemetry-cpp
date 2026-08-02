// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "opentelemetry/ext/http/server/http_server.h"
#include "opentelemetry/ext/http/server/socket_tools.h"

// The would-block path is exercised with a POSIX socketpair whose kernel send buffer is filled.
// Windows has no socketpair(); the shared HttpServer would-block decision logic these tests drive
// is platform-independent, so the coverage is provided on the POSIX runners. Windows-specific
// reactor notification behavior is outside this target.
#ifndef _WIN32

#  include <errno.h>
#  include <fcntl.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <unistd.h>
#  include <map>
#  include <string>
#  include <vector>

namespace
{

// sendMore() is protected because it is a reactor internal. A test subclass reaches it so the
// would-block branch can be driven directly: under the level-triggered reactor a single send()
// after a writable readiness event returns a positive count on the normal Linux path, so this
// branch is otherwise reached from the readable path on a backlogged keep-alive connection, which
// is hard to stage deterministically.
class SendMoreProbe : public HTTP_SERVER_NS::HttpServer
{
public:
  using HttpServer::Connection;
  using HttpServer::handleConnection;
  using HttpServer::m_connections;
  using HttpServer::m_reactor;
  using HttpServer::sendMore;

  // After a would-block, sendMore() must re-arm the socket for Writable | Closed so the reactor
  // calls back once the buffer drains. m_reactor is protected and Reactor::m_sockets is public, so
  // the test can read back the software interest flags addSocket() recorded.
  int reactorFlags(const SocketTools::Socket &socket)
  {
    for (const auto &data : m_reactor.m_sockets)
    {
      if (data.socket == socket)
      {
        return data.flags;
      }
    }
    return 0;
  }
};

// A nonblocking send() to a socket whose kernel send buffer is already full returns -1 with
// EWOULDBLOCK. sendMore() must treat that as transient: keep the unsent response intact and report
// there is more to send. Erasing on a negative count would compute erase(0, SIZE_MAX) and wipe the
// entire response.
TEST(HttpServerSendMoreTest, WouldBlockPreservesTheSendBuffer)
{
  int fds[2];
  ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);

  // A small send buffer plus a nonblocking sender fills quickly; the peer never reads.
  int sndbuf = 1024;
  ASSERT_EQ(::setsockopt(fds[0], SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf)), 0);
  const int fl = ::fcntl(fds[0], F_GETFL, 0);
  ASSERT_NE(fl, -1);
  ASSERT_EQ(::fcntl(fds[0], F_SETFL, fl | O_NONBLOCK), 0);

  // Fill the kernel send buffer so the next send() would block. Bounded so a misbehaving kernel
  // cannot spin here forever.
  const std::string filler(4096, 'x');
  bool blocked = false;
  for (int i = 0; i < 100000; ++i)
  {
    errno              = 0;
    const ssize_t sent = ::send(fds[0], filler.data(), filler.size(), 0);
    if (sent < 0 && errno == EINTR)
    {
      continue;
    }
    if (sent < 0 && errno == SocketTools::Socket::ErrorWouldBlock)
    {
      blocked = true;
      break;
    }
    ASSERT_GE(sent, 0);
  }
  ASSERT_TRUE(blocked) << "could not fill the socket send buffer";

  SendMoreProbe server;
  SendMoreProbe::Connection conn;
  conn.socket = SocketTools::Socket(fds[0]);
  const std::string response("HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nhello");
  conn.sendBuffer = response;

  const bool more = server.sendMore(conn);

  // sendMore() hit the would-block branch: the response is kept (not wiped) and there is more to
  // send.
  EXPECT_TRUE(more);
  EXPECT_EQ(conn.sendBuffer, response);
  // The socket is also re-armed for Writable | Closed so the reactor resumes the send once the
  // buffer drains; without this the response is kept but the connection would stall forever.
  EXPECT_EQ(server.reactorFlags(conn.socket),
            SocketTools::Reactor::Writable | SocketTools::Reactor::Closed);

  conn.socket.close();  // closes fds[0]
  ::close(fds[1]);
}

// A readiness notification can go stale before the callback runs, so a nonblocking recv() in
// onSocketReadable() can return -1 with EWOULDBLOCK even though the readable callback fired. That
// transient must be ignored (wait for the next readable event) rather than treated as end-of-stream
// that tears the connection down.
TEST(HttpServerReadableTest, WouldBlockKeepsTheConnection)
{
  int fds[2];
  ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);

  // The server side is nonblocking; the peer never writes, so recv() reports would-block.
  const int fl = ::fcntl(fds[0], F_GETFL, 0);
  ASSERT_NE(fl, -1);
  ASSERT_EQ(::fcntl(fds[0], F_SETFL, fl | O_NONBLOCK), 0);

  SendMoreProbe server;
  const SocketTools::Socket sock(fds[0]);

  // Stage a mid-request connection and arm it exactly as a freshly accepted connection would be.
  SendMoreProbe::Connection &conn = server.m_connections[sock];
  conn.socket                     = sock;
  conn.state                      = SendMoreProbe::Connection::ReceivingHeaders;
  server.m_reactor.addSocket(sock, SocketTools::Reactor::Readable | SocketTools::Reactor::Closed);

  // Drive the readable path through the reactor's public callback reference. Virtual dispatch
  // reaches the private onSocketReadable() override, so no production surface has to be widened.
  server.m_reactor.m_callback.onSocketReadable(sock);

  // The would-block recv() is transient: the connection stays in the map, the socket stays open,
  // and its reactor registration is untouched (still Readable | Closed) so a later readable event
  // can retry.
  EXPECT_TRUE(server.m_connections.find(sock) != server.m_connections.end());
  EXPECT_NE(::fcntl(fds[0], F_GETFD, 0), -1);
  EXPECT_EQ(server.reactorFlags(sock),
            SocketTools::Reactor::Readable | SocketTools::Reactor::Closed);

  ::close(fds[0]);
  ::close(fds[1]);
}

// After the interim "100 Continue" response has drained, the server must return to reading so it
// can receive the request body. addSocket() replaces (does not merge) the interest set, so a
// would-block during the interim send leaves the socket armed for Writable only; the
// Sending100Continue -> ReceivingBody transition must therefore restore Readable | Closed,
// otherwise the body is never read and the connection stalls.
TEST(HttpServer100ContinueTest, CompletedInterimResponseRestoresReadInterest)
{
  int fds[2];
  ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);
  const int fl = ::fcntl(fds[0], F_GETFL, 0);
  ASSERT_NE(fl, -1);
  ASSERT_EQ(::fcntl(fds[0], F_SETFL, fl | O_NONBLOCK), 0);

  SendMoreProbe server;
  SendMoreProbe::Connection conn;
  conn.socket = SocketTools::Socket(fds[0]);
  // The interim response has finished sending (empty sendBuffer) and a one-byte body is still
  // outstanding, so handleConnection() parks in ReceivingBody instead of running the request.
  conn.state = SendMoreProbe::Connection::Sending100Continue;
  conn.sendBuffer.clear();
  conn.receiveBuffer.clear();
  conn.contentLength = 1;

  // Simulate the Writable-only interest left behind when the interim send blocked.
  server.m_reactor.addSocket(conn.socket,
                             SocketTools::Reactor::Writable | SocketTools::Reactor::Closed);

  server.handleConnection(conn);

  // The interim send is complete, so the server advances to reading the body and restores read
  // interest.
  EXPECT_EQ(conn.state, SendMoreProbe::Connection::ReceivingBody);
  EXPECT_EQ(server.reactorFlags(conn.socket),
            SocketTools::Reactor::Readable | SocketTools::Reactor::Closed);

  ::close(fds[0]);
  ::close(fds[1]);
}

}  // namespace

#endif  // _WIN32

// The cases above drive POSIX socketpairs, so the Windows build of this target would otherwise
// contain no tests at all and pass vacuously. This one is declared on every platform to keep the
// binary non-empty and to give gtest_add_tests, which scans the source, a case the binary
// contains everywhere.
TEST(HttpServerNonBlockingIo, NotApplicableOnWindows)
{
#ifdef _WIN32
  GTEST_SKIP() << "the would-block paths are driven with a POSIX socketpair";
#else
  GTEST_SKIP() << "covered by the HttpServerSendMoreTest, HttpServerReadableTest and "
                  "HttpServer100ContinueTest cases above";
#endif
}
