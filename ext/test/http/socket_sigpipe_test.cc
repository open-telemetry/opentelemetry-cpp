// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>
#include <string>

#include "opentelemetry/ext/http/server/socket_tools.h"

// SIGPIPE only exists on POSIX. Windows has no such signal and the suppression code is compiled
// out there, so the POSIX tests live behind _WIN32 and Windows gets an explicit skipped test
// rather than an empty binary.
#ifndef _WIN32

#  include <errno.h>
#  include <pthread.h>  // IWYU pragma: keep  (legacy BSD declarations may require it)
#  include <signal.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <unistd.h>

namespace
{

// Owns the two descriptors of a socketpair so they are closed on every path, including when a test
// aborts on a fatal assertion. A descriptor handed to a SocketTools::Socket is released from this
// owner by setting its slot to -1, which leaves the Socket as the sole owner.
struct ScopedFd
{
  int fds[2]{-1, -1};

  ScopedFd()                            = default;
  ScopedFd(const ScopedFd &)            = delete;
  ScopedFd(ScopedFd &&)                 = delete;
  ScopedFd &operator=(const ScopedFd &) = delete;
  ScopedFd &operator=(ScopedFd &&)      = delete;

  ~ScopedFd()
  {
    if (fds[0] >= 0)
    {
      ::close(fds[0]);
    }
    if (fds[1] >= 0)
    {
      ::close(fds[1]);
    }
  }
};

// SocketTools::Socket is a non-owning wrapper with no destructor, so TearDown closes every socket a
// test may open and then restores the signal state. TearDown runs even when a test aborts on a
// fatal assertion, so a leaked descriptor, a leaked signal mask, or a queued SIGPIPE cannot pollute
// later tests sharing this binary.
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
    // A SIGPIPE pending before the test would make the signal-behavior cases impossible to
    // attribute, so treat it as a fatal precondition rather than an expectation.
    ASSERT_FALSE(SigPipeIsPending()) << "a SIGPIPE was already pending before the test";
  }

  void TearDown() override
  {
    // Sockets are always closed here because Socket keeps no ownership of the descriptor; the
    // mask_changed_ guard below concerns only the signal state.
    CloseIfValid(accepted_);
    CloseIfValid(client_);
    CloseIfValid(listener_);
    CloseIfValid(socket_);

    if (!mask_changed_)
    {
      return;  // SetUp never blocked SIGPIPE, so there is nothing to drain or restore, and
               // sigwait() on an unblocked signal would be unsafe.
    }
    DrainPendingSigPipe();
    EXPECT_EQ(pthread_sigmask(SIG_SETMASK, &previous_, nullptr), 0);
  }

  static void CloseIfValid(SocketTools::Socket &sock)
  {
    if (!sock.invalid())
    {
      sock.close();
    }
  }

  static bool SigPipeIsPending()
  {
    sigset_t pending;
    sigemptyset(&pending);
    EXPECT_EQ(sigpending(&pending), 0);
    return sigismember(&pending, SIGPIPE) == 1;
  }

  // Each case runs in its own process under gtest_add_tests, so a signal-behavior test must
  // establish for itself that SIGPIPE can be observed. A linked library such as gRPC may set
  // SIG_IGN process-wide, and an ignored signal is discarded before it can pend, so a broken-pipe
  // write would prove nothing; the caller skips instead of passing vacuously.
  static bool SigPipeIsIgnored()
  {
    struct sigaction current = {};
    EXPECT_EQ(::sigaction(SIGPIPE, nullptr, &current), 0);
    return current.sa_handler == SIG_IGN;
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
  SocketTools::Socket socket_;
  SocketTools::Socket listener_;
  SocketTools::Socket client_;
  SocketTools::Socket accepted_;
};

// Baseline: a raw send() with no suppression to a closed peer must raise SIGPIPE. It runs in its
// own process and guards itself with SigPipeIsIgnored(), so it is an independent case rather than a
// precondition the suppression tests below rely on.
TEST_F(SocketSigPipeTest, RawSendToClosedPeerRaisesSigPipe)
{
  if (SigPipeIsIgnored())
  {
    GTEST_SKIP() << "SIGPIPE is ignored in this process; a broken-pipe write cannot be observed";
  }

  ScopedFd pair;
  ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, pair.fds), 0);
  ::close(pair.fds[1]);
  pair.fds[1] = -1;

  const char byte      = 'x';
  errno                = 0;
  const ssize_t result = ::send(pair.fds[0], &byte, 1, 0);
  const int saved      = errno;  // save before any assertion can perturb errno
  EXPECT_EQ(result, -1);
  EXPECT_EQ(saved, EPIPE);
  EXPECT_TRUE(SigPipeIsPending()) << "the environment did not deliver SIGPIPE, so the suppression "
                                     "tests would pass vacuously";
  DrainPendingSigPipe();
}

// The wrapper's send() passes MSG_NOSIGNAL wherever the platform defines it, so a write to a closed
// peer reports EPIPE without raising SIGPIPE. The socket is wrapped from a raw fd and
// suppressSigPipe() is not called, so MSG_NOSIGNAL is the only mechanism exercised.
TEST_F(SocketSigPipeTest, WrapperSendToClosedPeerUsesMsgNoSignal)
{
#  ifndef MSG_NOSIGNAL
  GTEST_SKIP() << "MSG_NOSIGNAL is not available on this platform";
#  else
  if (SigPipeIsIgnored())
  {
    GTEST_SKIP() << "SIGPIPE is ignored in this process; a broken-pipe write cannot be observed";
  }

  ScopedFd pair;
  ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, pair.fds), 0);
  socket_     = SocketTools::Socket(pair.fds[0]);
  pair.fds[0] = -1;  // ownership handed to socket_, which TearDown closes
  ::close(pair.fds[1]);
  pair.fds[1] = -1;

  const char byte = 'x';
  errno           = 0;
  const int sent  = socket_.send(&byte, 1);
  const int saved = errno;  // save before any assertion can perturb errno
  EXPECT_EQ(sent, -1);
  EXPECT_EQ(saved, EPIPE);
  EXPECT_FALSE(SigPipeIsPending());
#  endif
}

// Where the platform offers SO_NOSIGPIPE (macOS, the BSDs), suppressSigPipe() sets it at the socket
// level, guarding even a write that bypasses the wrapper's per-call MSG_NOSIGNAL. A raw ::send()
// with flags 0 exercises exactly that path: EPIPE, and no SIGPIPE queued.
TEST_F(SocketSigPipeTest, SuppressSigPipeGuardsRawSendToClosedPeer)
{
#  ifndef SO_NOSIGPIPE
  GTEST_SKIP() << "SO_NOSIGPIPE is not available on this platform";
#  else
  if (SigPipeIsIgnored())
  {
    GTEST_SKIP() << "SIGPIPE is ignored in this process; a broken-pipe write cannot be observed";
  }

  ScopedFd pair;
  ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, pair.fds), 0);
  const int fd = pair.fds[0];
  socket_      = SocketTools::Socket(fd);
  pair.fds[0]  = -1;  // ownership handed to socket_, which TearDown closes
  socket_.suppressSigPipe();
  ::close(pair.fds[1]);
  pair.fds[1] = -1;

  const char byte      = 'x';
  errno                = 0;
  const ssize_t result = ::send(fd, &byte, 1, 0);  // raw send bypasses the wrapper's MSG_NOSIGNAL
  const int saved      = errno;                    // save before any assertion can perturb errno
  EXPECT_EQ(result, -1);
  EXPECT_EQ(saved, EPIPE);
  EXPECT_FALSE(SigPipeIsPending());
#  endif
}

// Where the platform offers SO_NOSIGPIPE (macOS, the BSDs), the (af, type, proto) constructor sets
// it through suppressSigPipe(). Where it does not (Linux, which suppresses via MSG_NOSIGNAL) the
// test skips at runtime, so it stays a registered test on every POSIX platform.
TEST_F(SocketSigPipeTest, CreatedSocketCarriesSoNoSigPipe)
{
#  ifndef SO_NOSIGPIPE
  GTEST_SKIP() << "SO_NOSIGPIPE is not available on this platform";
#  else
  socket_ = SocketTools::Socket(AF_INET, SOCK_STREAM, 0);
  ASSERT_FALSE(socket_.invalid());

  int value        = 0;
  socklen_t length = sizeof(value);
  ASSERT_EQ(::getsockopt(socket_, SOL_SOCKET, SO_NOSIGPIPE, &value, &length), 0);
  EXPECT_EQ(value, 1);
#  endif
}

// accept() must set SO_NOSIGPIPE on the socket it returns. The listener is wrapped from a raw fd
// (the Socket(Type) constructor does not call the helper) and its own SO_NOSIGPIPE is cleared to 0
// first, so the accepted socket reading back 1 proves accept()'s own suppressSigPipe() call rather
// than inheritance from the listener.
TEST_F(SocketSigPipeTest, AcceptedSocketCarriesSoNoSigPipe)
{
#  ifndef SO_NOSIGPIPE
  GTEST_SKIP() << "SO_NOSIGPIPE is not available on this platform";
#  else
  listener_ = SocketTools::Socket(::socket(AF_INET, SOCK_STREAM, 0));
  ASSERT_FALSE(listener_.invalid());

  // Clear SO_NOSIGPIPE on the listener and confirm it reads back 0, so a value of 1 on the accepted
  // socket can only come from accept()'s own suppressSigPipe() call.
  int listener_flag = 0;
  ASSERT_EQ(
      ::setsockopt(listener_, SOL_SOCKET, SO_NOSIGPIPE, &listener_flag, sizeof(listener_flag)), 0);
  socklen_t listener_length = sizeof(listener_flag);
  ASSERT_EQ(::getsockopt(listener_, SOL_SOCKET, SO_NOSIGPIPE, &listener_flag, &listener_length), 0);
  ASSERT_EQ(listener_flag, 0);

  SocketTools::SocketAddr bind_addr(SocketTools::SocketAddr::Loopback, 0);  // 127.0.0.1, ephemeral
  ASSERT_TRUE(listener_.bind(bind_addr));
  ASSERT_TRUE(listener_.listen(1));

  SocketTools::SocketAddr bound;
  ASSERT_TRUE(listener_.getsockname(bound));

  client_ = SocketTools::Socket(AF_INET, SOCK_STREAM, 0);
  ASSERT_FALSE(client_.invalid());
  SocketTools::SocketAddr server_addr(SocketTools::SocketAddr::Loopback, bound.port());
  // Blocking connect on loopback completes into the listen backlog before accept() runs.
  ASSERT_TRUE(client_.connect(server_addr));

  SocketTools::SocketAddr peer;
  ASSERT_TRUE(listener_.accept(accepted_, peer));  // accept() calls suppressSigPipe() on the result

  int value        = 0;
  socklen_t length = sizeof(value);
  ASSERT_EQ(::getsockopt(accepted_, SOL_SOCKET, SO_NOSIGPIPE, &value, &length), 0);
  EXPECT_EQ(value, 1);
#  endif
}

}  // namespace

#endif  // _WIN32

// Windows has no POSIX SIGPIPE, so the fixture and its tests above are POSIX-only. This test is
// declared on every platform to keep the Windows target non-empty and to give gtest_add_tests,
// which scans the source, a test the binary contains everywhere.
TEST(SocketSigPipe, NotApplicableOnWindows)
{
#ifdef _WIN32
  GTEST_SKIP() << "Windows does not generate POSIX SIGPIPE";
#else
  GTEST_SKIP() << "POSIX SIGPIPE suppression is covered by the SocketSigPipeTest fixture";
#endif
}
