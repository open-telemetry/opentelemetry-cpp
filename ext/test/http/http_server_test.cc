// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "opentelemetry/ext/http/server/http_server.h"

// The would-block path is exercised with a POSIX socketpair whose kernel send buffer is filled.
// Windows has no socketpair(); the reactor's would-block handling is identical, so the coverage is
// provided on the POSIX runners.
#ifndef _WIN32

#  include <errno.h>
#  include <fcntl.h>
#  include <sys/socket.h>
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
  using HttpServer::sendMore;
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
  ::setsockopt(fds[0], SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));
  const int fl = ::fcntl(fds[0], F_GETFL, 0);
  ASSERT_EQ(::fcntl(fds[0], F_SETFL, fl | O_NONBLOCK), 0);

  // Fill the kernel send buffer so the next send() would block. Bounded so a misbehaving kernel
  // cannot spin here forever.
  const std::string filler(4096, 'x');
  bool blocked = false;
  for (int i = 0; i < 100000; ++i)
  {
    errno              = 0;
    const ssize_t sent = ::send(fds[0], filler.data(), filler.size(), MSG_NOSIGNAL);
    if (sent < 0 && (errno == EWOULDBLOCK || errno == EAGAIN))
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

  conn.socket.close();  // closes fds[0]
  ::close(fds[1]);
}

}  // namespace

#endif  // _WIN32
