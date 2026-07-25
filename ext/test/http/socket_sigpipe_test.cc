// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include "opentelemetry/ext/http/server/socket_tools.h"

// SIGPIPE only exists on POSIX. Windows has no such signal and the suppression code is compiled
// out there, so the POSIX tests live behind _WIN32 and Windows gets an explicit skipped test
// rather than an empty binary.
#ifndef _WIN32

#  include <errno.h>
#  include <pthread.h>
#  include <signal.h>
#  include <sys/socket.h>
#  include <unistd.h>

namespace
{

// The mask change and any pending SIGPIPE are cleaned up in TearDown, which runs even when a test
// aborts on a fatal assertion. That matters here: Socket is a non-owning wrapper with no
// destructor, and a leaked signal mask or a queued SIGPIPE would pollute later tests sharing this
// binary.
class SocketSigPipeTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    sigemptyset(&blocked_);
    sigaddset(&blocked_, SIGPIPE);
    // Observe SIGPIPE through sigpending() instead of the default disposition, so a stray signal
    // cannot kill the runner and the test does not depend on the process-wide handler.
    ASSERT_EQ(pthread_sigmask(SIG_BLOCK, &blocked_, &previous_), 0);
    mask_changed_ = true;
    EXPECT_FALSE(SigPipeIsPending()) << "a SIGPIPE was already pending before the test";
  }

  void TearDown() override
  {
    DrainPendingSigPipe();
    if (mask_changed_)
    {
      EXPECT_EQ(pthread_sigmask(SIG_SETMASK, &previous_, nullptr), 0);
    }
  }

  static bool SigPipeIsPending()
  {
    sigset_t pending;
    sigemptyset(&pending);
    EXPECT_EQ(sigpending(&pending), 0);
    return sigismember(&pending, SIGPIPE) == 1;
  }

  void DrainPendingSigPipe()
  {
    if (SigPipeIsPending())
    {
      int signo = 0;
      EXPECT_EQ(sigwait(&blocked_, &signo), 0);
      EXPECT_EQ(signo, SIGPIPE);
    }
  }

  sigset_t blocked_{};
  sigset_t previous_{};
  bool mask_changed_{false};
};

// Writing to a socket whose peer has closed must report EPIPE without raising SIGPIPE, whose
// default disposition would terminate the host process. A closed peer fails the write immediately,
// so one byte is enough. A negative control on a raw socket first proves this thread really does
// observe a SIGPIPE, so the suppressed result is meaningful rather than vacuous.
TEST_F(SocketSigPipeTest, WriteToClosedPeerReportsEpipeWithoutRaisingSigPipe)
{
#  if !defined(MSG_NOSIGNAL) && !defined(SO_NOSIGPIPE)
  GTEST_SKIP()
      << "no SIGPIPE-suppression mechanism (MSG_NOSIGNAL or SO_NOSIGPIPE) on this platform";
#  else
  const char byte = 'x';

  // Negative control: a raw send() with no suppression to a closed peer raises SIGPIPE.
  {
    int raw[2];
    ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, raw), 0);
    ::close(raw[1]);
    errno           = 0;
    const ssize_t r = ::send(raw[0], &byte, 1, 0);
    EXPECT_EQ(r, -1);
    EXPECT_EQ(errno, EPIPE);
    EXPECT_TRUE(SigPipeIsPending()) << "the environment does not deliver SIGPIPE, so the "
                                       "suppressed case below would pass vacuously";
    DrainPendingSigPipe();
    ::close(raw[0]);
  }

  // The wrapper suppresses it: EPIPE, and no SIGPIPE queued.
  int fds[2];
  ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);
  SocketTools::Socket sender(fds[0]);
  // Production sockets get SO_NOSIGPIPE from accept()/the (af,type,proto) ctor. Apply it here too,
  // since on macOS and the BSDs, where MSG_NOSIGNAL is not defined, that option is the layer under
  // test.
  sender.suppressSigPipe();
  ::close(fds[1]);

  errno           = 0;
  const int sent  = sender.send(&byte, 1);
  const int saved = errno;
  EXPECT_EQ(sent, -1);
  EXPECT_EQ(saved, EPIPE);
  EXPECT_FALSE(SigPipeIsPending());

  sender.close();
#  endif
}

#  ifdef SO_NOSIGPIPE
// Where the platform offers SO_NOSIGPIPE (macOS, the BSDs), the (af,type,proto) constructor sets
// it.
TEST_F(SocketSigPipeTest, CreatedSocketCarriesSoNoSigPipe)
{
  SocketTools::Socket sock(AF_INET, SOCK_STREAM, 0);
  ASSERT_FALSE(sock.invalid());

  int value        = 0;
  socklen_t length = sizeof(value);
  ASSERT_EQ(::getsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &value, &length), 0);
  EXPECT_EQ(value, 1);

  sock.close();
}

// accept() must set SO_NOSIGPIPE on the socket it returns. The listener is wrapped from a raw fd
// (the Socket(Type) constructor does not call the helper) and the test drives the wrapper's own
// accept(), so a pass proves accept()'s suppressSigPipe() call rather than option inheritance.
TEST_F(SocketSigPipeTest, AcceptedSocketCarriesSoNoSigPipe)
{
  SocketTools::Socket listener(::socket(AF_INET, SOCK_STREAM, 0));
  ASSERT_FALSE(listener.invalid());
  listener.setReuseAddr();

  SocketTools::SocketAddr bind_addr(SocketTools::SocketAddr::Loopback, 0);  // 127.0.0.1, ephemeral
  ASSERT_TRUE(listener.bind(bind_addr));
  ASSERT_TRUE(listener.listen(1));

  SocketTools::SocketAddr bound;
  ASSERT_TRUE(listener.getsockname(bound));

  SocketTools::Socket client(AF_INET, SOCK_STREAM, 0);
  ASSERT_FALSE(client.invalid());
  SocketTools::SocketAddr server_addr(SocketTools::SocketAddr::Loopback, bound.port());
  // Blocking connect on loopback completes into the listen backlog before accept() runs.
  ASSERT_TRUE(client.connect(server_addr));

  SocketTools::Socket accepted;
  SocketTools::SocketAddr peer;
  ASSERT_TRUE(listener.accept(accepted, peer));  // accept() calls suppressSigPipe() on the result

  int value        = 0;
  socklen_t length = sizeof(value);
  ASSERT_EQ(::getsockopt(accepted, SOL_SOCKET, SO_NOSIGPIPE, &value, &length), 0);
  EXPECT_EQ(value, 1);

  accepted.close();
  client.close();
  listener.close();
}
#  endif  // SO_NOSIGPIPE

}  // namespace

#else  // _WIN32

TEST(SocketSigPipeTest, NotApplicableOnWindows)
{
  GTEST_SKIP() << "Windows does not generate POSIX SIGPIPE";
}

#endif  // _WIN32
