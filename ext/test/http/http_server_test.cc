// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "opentelemetry/ext/http/server/http_server.h"
#include "opentelemetry/ext/http/server/socket_tools.h"

// The would-block path is exercised with a POSIX socketpair whose kernel send buffer is filled.
// Windows has no socketpair(); the reactor's would-block handling is identical, so the coverage is
// provided on the POSIX runners.
#ifndef _WIN32

#  include <errno.h>
#  include <fcntl.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <unistd.h>
#  include <string>

namespace
{

// sendMore() is protected because it is a reactor internal. A test subclass reaches it so the
// would-block branch can be driven directly: under the level-triggered reactor a single send()
// after a readiness event returns a positive count, so this branch is otherwise only reached from
// the readable path on a backlogged keep-alive connection, which is hard to stage
// deterministically.
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
// EWOULDBLOCK. The old sendMore() fell through to sendBuffer.erase(0, sent) with sent == -1, which
// converts to erase(0, SIZE_MAX) and wipes the entire unsent response. sendMore() must instead keep
// the buffer intact and report that there is more to send.
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

  // sendMore() hit the would-block branch: the response is kept, not wiped, and there is more to
  // send. The old bug emptied conn.sendBuffer here.
  EXPECT_TRUE(more);
  EXPECT_EQ(conn.sendBuffer, response);
  // The socket is also re-armed for Writable | Closed so the reactor resumes the send once the
  // buffer drains; without this the response is kept but the connection would stall forever.
  EXPECT_EQ(server.reactorFlags(conn.socket),
            SocketTools::Reactor::Writable | SocketTools::Reactor::Closed);

  conn.socket.close();  // closes fds[0]
  ::close(fds[1]);
}

// A readable event can fire on a nonblocking socket that has no data yet: a spurious wakeup, or a
// peer that connected but has not written. recv() then returns -1 with EWOULDBLOCK. The old
// onSocketReadable() treated every non-positive recv() as end-of-stream and tore the connection
// down; a transient would-block must instead be ignored so the next readable event can retry.
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

  // The would-block recv() is transient: the connection stays registered and the socket stays open.
  // The old bug erased the connection and closed fds[0] here.
  EXPECT_TRUE(server.m_connections.find(sock) != server.m_connections.end());
  EXPECT_NE(::fcntl(fds[0], F_GETFD, 0), -1);

  ::close(fds[0]);
  ::close(fds[1]);
}

// After the interim "100 Continue" response has drained, the server must return to reading so it
// can receive the request body. addSocket() replaces (does not merge) the interest set, so a
// would-block during the interim send leaves the socket armed for Writable only. The
// Sending100Continue -> ReceivingBody transition must restore Readable | Closed; the old code
// advanced to ReceivingBody while still armed Writable-only, so the body was never read and the
// connection stalled.
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
  // interest. Without the fix the socket stays armed Writable-only and the body never arrives.
  EXPECT_EQ(conn.state, SendMoreProbe::Connection::ReceivingBody);
  EXPECT_EQ(server.reactorFlags(conn.socket),
            SocketTools::Reactor::Readable | SocketTools::Reactor::Closed);

  ::close(fds[0]);
  ::close(fds[1]);
}

}  // namespace

#endif  // _WIN32
