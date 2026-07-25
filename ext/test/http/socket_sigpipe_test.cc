// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "opentelemetry/ext/http/server/socket_tools.h"

// SIGPIPE only exists on POSIX. Windows has no such signal and the suppression code is compiled
// out there, so the whole fixture is POSIX-only.
#ifndef _WIN32

#  include <errno.h>
#  include <pthread.h>
#  include <signal.h>
#  include <sys/socket.h>
#  include <unistd.h>

namespace
{

// Writing to a socket whose peer has closed must report the error through errno rather than
// raising SIGPIPE, whose default disposition would terminate the host process; send() passes
// MSG_NOSIGNAL for exactly this reason. The peer is closed rather than merely idle, so a single
// byte is enough: the write fails immediately with EPIPE instead of being buffered.
TEST(SocketSigPipeTest, WriteToClosedPeerReportsEpipeWithoutRaisingSigPipe)
{
  // Observe SIGPIPE through sigpending() rather than the default disposition, so the test neither
  // depends on nor changes the process-wide handler and cannot be killed by a stray signal.
  sigset_t blocked, previous;
  sigemptyset(&blocked);
  sigaddset(&blocked, SIGPIPE);
  ASSERT_EQ(pthread_sigmask(SIG_BLOCK, &blocked, &previous), 0);

  int fds[2];
  ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);
  SocketTools::Socket sender(fds[0]);
  ::close(fds[1]);  // the peer is gone, so any further write is a broken pipe

  const char byte = 'x';
  errno           = 0;
  const int sent  = sender.send(&byte, 1);
  const int saved = errno;

  EXPECT_EQ(sent, -1);
  EXPECT_EQ(saved, EPIPE);

  sigset_t pending;
  sigemptyset(&pending);
  ASSERT_EQ(sigpending(&pending), 0);
  EXPECT_FALSE(sigismember(&pending, SIGPIPE));

  // If the fix regressed and a SIGPIPE was queued, drain it so it cannot escape the test.
  if (sigismember(&pending, SIGPIPE))
  {
    int signo = 0;
    sigwait(&blocked, &signo);
  }

  sender.close();
  pthread_sigmask(SIG_SETMASK, &previous, nullptr);
}

#  ifdef SO_NOSIGPIPE
// Where the platform offers SO_NOSIGPIPE (macOS and the BSDs), a socket created through the wrapper
// should carry it as the best-effort second layer described on suppressSigPipe().
TEST(SocketSigPipeTest, CreatedSocketCarriesSoNoSigPipe)
{
  SocketTools::Socket sock(AF_INET, SOCK_STREAM, 0);
  ASSERT_FALSE(sock.invalid());

  int value        = 0;
  socklen_t length = sizeof(value);
  ASSERT_EQ(::getsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &value, &length), 0);
  EXPECT_EQ(value, 1);

  sock.close();
}
#  endif  // SO_NOSIGPIPE

}  // namespace

#endif  // _WIN32
