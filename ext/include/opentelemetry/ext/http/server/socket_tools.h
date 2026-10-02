// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0
#pragma once
 
#include <algorithm>
#include <atomic>
#include <cassert>
#include <cerrno>
#include <chrono>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
 
#ifdef _WIN32
 
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <winsock2.h>
#  include <ws2tcpip.h>  // inet_pton / inet_ntop
 
#  undef min
#  undef max
#  pragma comment(lib, "ws2_32.lib")
 
#else
 
#  include <unistd.h>
 
#  ifdef __linux__
#    include <sys/epoll.h>
#  endif
 
#  ifdef __APPLE__
#    include <sys/event.h>
#    include <sys/time.h>
#    include <sys/types.h>
#  endif
 
// Common POSIX headers for Linux and Mac OS X
#  include <arpa/inet.h>
#  include <fcntl.h>
#  include <netdb.h>
#  include <netinet/in.h>
#  include <netinet/tcp.h>
#  include <sys/socket.h>
 
#endif
 
#if defined(HAVE_CONSOLE_LOG) && !defined(LOG_DEBUG)
#  define LOG_DEBUG(fmt_, ...) std::printf(" " fmt_ "\n", ##__VA_ARGS__)
#  define LOG_TRACE(fmt_, ...) std::printf(" " fmt_ "\n", ##__VA_ARGS__)
#  define LOG_INFO(fmt_, ...) std::printf(" " fmt_ "\n", ##__VA_ARGS__)
#  define LOG_WARN(fmt_, ...) std::printf(" " fmt_ "\n", ##__VA_ARGS__)
#  define LOG_ERROR(fmt_, ...) std::printf(" " fmt_ "\n", ##__VA_ARGS__)
#endif
 
#ifndef LOG_DEBUG
#  define LOG_DEBUG(fmt_, ...)
#  define LOG_TRACE(fmt_, ...)
#  define LOG_INFO(fmt_, ...)
#  define LOG_WARN(fmt_, ...)
#  define LOG_ERROR(fmt_, ...)
#endif
 
namespace common
{
 
/// <summary>
/// A simple thread, derived class overloads onThread() method.
/// NOTE: the most-derived class MUST call joinThread() in its own destructor, because by the
/// time ~Thread() runs the derived part (and onThread()) no longer exists.
/// </summary>
struct Thread
{
  std::thread m_thread;
  std::atomic<bool> m_terminate{false};
 
  Thread()                          = default;
  Thread(const Thread &)            = delete;
  Thread(Thread &&)                 = delete;
  Thread &operator=(const Thread &) = delete;
  Thread &operator=(Thread &&)      = delete;
 
  void startThread()
  {
    if (m_thread.joinable())
    {
      return;  // already running
    }
    m_terminate = false;
    m_thread    = std::thread([this]() { this->onThread(); });
  }
 
  void joinThread()
  {
    m_terminate = true;
    if (m_thread.joinable())
    {
      m_thread.join();
    }
  }
 
  bool shouldTerminate() const { return m_terminate; }
 
  virtual void onThread() = 0;
 
  virtual ~Thread() noexcept = default;
};
 
}  // namespace common
 
namespace SocketTools
{
 
#ifdef _WIN32
struct WsaInitializer
{
  WsaInitializer()
  {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
      LOG_ERROR("WSAStartup failed");
    }
  }
 
  ~WsaInitializer() { WSACleanup(); }
};
 
#  if defined(__cpp_inline_variables) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
inline WsaInitializer g_wsaInitializer;
#  else
static WsaInitializer g_wsaInitializer;  // reference counted by WSAStartup, safe if duplicated
#  endif
#endif
 
/// <summary>
/// Encapsulation of sockaddr_storage for safe alignment and protocol independence.
/// Accepts "a.b.c.d[:port]", "[v6addr][:port]" or a bare "v6addr".
/// </summary>
struct SocketAddr
{
  static uint32_t const Loopback = 0x7F000001;
 
  sockaddr_storage m_data{};
  socklen_t m_len{sizeof(sockaddr_storage)};
 
  SocketAddr()
  {
    std::memset(&m_data, 0, sizeof(m_data));
    m_data.ss_family = AF_UNSPEC;
  }
 
  SocketAddr(uint32_t addr, uint16_t port)
  {
    std::memset(&m_data, 0, sizeof(m_data));
    sockaddr_in inet4{};
    inet4.sin_family      = AF_INET;
    inet4.sin_port        = htons(port);
    inet4.sin_addr.s_addr = htonl(addr);
 
    std::memcpy(&m_data, &inet4, sizeof(inet4));
    m_len = sizeof(sockaddr_in);
  }
 
  SocketAddr(char const *addr)
  {
    std::memset(&m_data, 0, sizeof(m_data));
    m_data.ss_family = AF_UNSPEC;
 
    if (addr == nullptr)
    {
      LOG_WARN("SocketAddr: cannot parse a null address");
      return;
    }
 
    bool ok = false;
    if (addr[0] == '[')
    {
      // "[v6]" or "[v6]:port"
      char const *close = std::strchr(addr, ']');
      if (close != nullptr && close > addr + 1)
      {
        uint16_t port = 0;
        ok            = true;
        if (close[1] == ':')
        {
          ok = parsePort(close + 2, port);
        }
        else if (close[1] != '\0')
        {
          ok = false;
        }
        if (ok)
        {
          ok = assignV6(addr + 1, static_cast<size_t>(close - addr - 1), port);
        }
      }
    }
    else
    {
      char const *first = std::strchr(addr, ':');
      char const *last  = std::strrchr(addr, ':');
      if (first != nullptr && first != last)
      {
        // more than one colon and no brackets: bare IPv6, no port
        ok = assignV6(addr, std::strlen(addr), 0);
      }
      else
      {
        char const *hostEnd = first ? first : addr + std::strlen(addr);
        uint16_t port       = 0;
        ok                  = true;
        if (first != nullptr)
        {
          ok = parsePort(first + 1, port);
        }
        if (ok)
        {
          ok = assignV4(addr, static_cast<size_t>(hostEnd - addr), port);
        }
      }
    }
 
    if (!ok)
    {
      std::memset(&m_data, 0, sizeof(m_data));
      m_data.ss_family = AF_UNSPEC;
      m_len            = sizeof(sockaddr_storage);
      LOG_WARN("SocketAddr: cannot parse address");
    }
  }
 
  operator sockaddr *() { return reinterpret_cast<sockaddr *>(&m_data); }
 
  operator const sockaddr *() const { return reinterpret_cast<const sockaddr *>(&m_data); }
 
  socklen_t length() const { return m_len; }
 
  int port() const
  {
    switch (m_data.ss_family)
    {
      case AF_INET: {
        sockaddr_in inet4{};
        std::memcpy(&inet4, &m_data, sizeof(inet4));
        return ntohs(inet4.sin_port);
      }
      case AF_INET6: {
        sockaddr_in6 inet6{};
        std::memcpy(&inet6, &m_data, sizeof(inet6));
        return ntohs(inet6.sin6_port);
      }
      default:
        return -1;
    }
  }
 
  std::string toString() const
  {
    std::ostringstream os;
    char buf[INET6_ADDRSTRLEN] = {};
 
    switch (m_data.ss_family)
    {
      case AF_INET: {
        sockaddr_in inet4{};
        std::memcpy(&inet4, &m_data, sizeof(inet4));
        if (::inet_ntop(AF_INET, &inet4.sin_addr, buf, sizeof(buf)) == nullptr)
        {
          buf[0] = '\0';
        }
        os << buf << ':' << ntohs(inet4.sin_port);
        break;
      }
      case AF_INET6: {
        sockaddr_in6 inet6{};
        std::memcpy(&inet6, &m_data, sizeof(inet6));
        if (::inet_ntop(AF_INET6, &inet6.sin6_addr, buf, sizeof(buf)) == nullptr)
        {
          buf[0] = '\0';
        }
        os << '[' << buf << "]:" << ntohs(inet6.sin6_port);
        break;
      }
      default:
        os << "[?AF?" << static_cast<int>(m_data.ss_family) << ']';
    }
    return os.str();
  }
 
private:
  static bool parsePort(char const *p, uint16_t &out)
  {
    if (p == nullptr || *p == '\0')
    {
      return false;
    }
    uint32_t value = 0;
    for (; *p != '\0'; ++p)
    {
      if (*p < '0' || *p > '9')
      {
        return false;
      }
      value = value * 10u + static_cast<uint32_t>(*p - '0');
      if (value > 65535u)
      {
        return false;
      }
    }
    out = static_cast<uint16_t>(value);
    return true;
  }
 
  bool assignV4(char const *host, size_t len, uint16_t port)
  {
    if (len < 1 || len > 15)
    {
      return false;
    }
    char buf[16];
    std::memcpy(buf, host, len);
    buf[len] = '\0';
 
    sockaddr_in parsed{};
    parsed.sin_family = AF_INET;
    if (::inet_pton(AF_INET, buf, &parsed.sin_addr) != 1)
    {
      return false;
    }
    parsed.sin_port = htons(port);
    std::memset(&m_data, 0, sizeof(m_data));
    std::memcpy(&m_data, &parsed, sizeof(parsed));
    m_len = sizeof(sockaddr_in);
    return true;
  }
 
  bool assignV6(char const *host, size_t len, uint16_t port)
  {
    if (len < 2 || len >= INET6_ADDRSTRLEN)
    {
      return false;
    }
    char buf[INET6_ADDRSTRLEN];
    std::memcpy(buf, host, len);
    buf[len] = '\0';
 
    sockaddr_in6 parsed{};
    parsed.sin6_family = AF_INET6;
    if (::inet_pton(AF_INET6, buf, &parsed.sin6_addr) != 1)
    {
      return false;
    }
    parsed.sin6_port = htons(port);
    std::memset(&m_data, 0, sizeof(m_data));
    std::memcpy(&m_data, &parsed, sizeof(parsed));
    m_len = sizeof(sockaddr_in6);
    return true;
  }
};
 
/// <summary>
/// Encapsulation of a socket (non-exclusive ownership: the destructor does NOT close it)
/// </summary>
struct Socket
{
#ifdef _WIN32
  using Type                = SOCKET;
  static Type const Invalid = INVALID_SOCKET;
  using IoLen               = int;
#else
  using Type                = int;
  static Type const Invalid = -1;
  using IoLen               = size_t;
#endif
 
  Type m_sock;
 
  Socket(Type sock = Invalid) : m_sock(sock) {}
 
  Socket(int af, int type, int proto) : m_sock(::socket(af, type, proto))
  {
    if (!invalid())
    {
      setNoSigPipe();
    }
  }
 
  operator Socket::Type() const { return m_sock; }
 
  bool operator==(Socket const &other) const { return (m_sock == other.m_sock); }
 
  bool operator!=(Socket const &other) const { return (m_sock != other.m_sock); }
 
  bool operator<(Socket const &other) const { return (m_sock < other.m_sock); }
 
  bool invalid() const { return (m_sock == Invalid); }
 
  bool setNonBlocking()
  {
    if (invalid())
    {
      return false;
    }
#ifdef _WIN32
    u_long value = 1;
    return (::ioctlsocket(m_sock, FIONBIO, &value) == 0);
#else
    int flags = ::fcntl(m_sock, F_GETFL, 0);
    if (flags == -1)
    {
      return false;
    }
    return (::fcntl(m_sock, F_SETFL, flags | O_NONBLOCK) != -1);
#endif
  }
 
  bool setReuseAddr()
  {
    if (invalid())
    {
      return false;
    }
#ifdef _WIN32
    BOOL value = TRUE;
#else
    int value = 1;
#endif
    return (::setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&value),
                         sizeof(value)) == 0);
  }
 
  bool setNoDelay()
  {
    if (invalid())
    {
      return false;
    }
#ifdef _WIN32
    BOOL value = TRUE;
#else
    int value = 1;
#endif
    return (::setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char *>(&value),
                         sizeof(value)) == 0);
  }
 
  /// Prevent SIGPIPE on macOS (Linux uses MSG_NOSIGNAL in send(); Windows has no SIGPIPE).
  bool setNoSigPipe()
  {
#if defined(__APPLE__) && defined(SO_NOSIGPIPE)
    int value = 1;
    return (::setsockopt(m_sock, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(value)) == 0);
#else
    return true;
#endif
  }
 
  bool connect(SocketAddr const &addr)
  {
    if (invalid())
    {
      return false;
    }
    return (::connect(m_sock, addr, addr.length()) == 0);
  }
 
  void close()
  {
    if (invalid())
    {
      return;
    }
#ifdef _WIN32
    ::closesocket(m_sock);
#else
    ::close(m_sock);
#endif
    m_sock = Invalid;
  }
 
  int recv(void *buffer, unsigned size)
  {
    if (invalid())
    {
      return -1;
    }
    const IoLen len = clampLength(size);
    for (;;)
    {
      auto result = ::recv(m_sock, reinterpret_cast<char *>(buffer), len, 0);
      if (result < 0 && interrupted())
      {
        continue;
      }
      return static_cast<int>(result);
    }
  }
 
  int send(void const *buffer, unsigned size)
  {
    if (invalid())
    {
      return -1;
    }
#ifdef MSG_NOSIGNAL
    const int flags = MSG_NOSIGNAL;
#else
    const int flags = 0;
#endif
    const IoLen len = clampLength(size);
    for (;;)
    {
      auto result = ::send(m_sock, reinterpret_cast<char const *>(buffer), len, flags);
      if (result < 0 && interrupted())
      {
        continue;
      }
      return static_cast<int>(result);
    }
  }
 
  bool bind(SocketAddr const &addr)
  {
    if (invalid())
    {
      return false;
    }
    return (::bind(m_sock, addr, addr.length()) == 0);
  }
 
  bool getsockname(SocketAddr &addr) const
  {
    if (invalid())
    {
      return false;
    }
    socklen_t addrlen = sizeof(addr.m_data);
    if (::getsockname(m_sock, addr, &addrlen) == 0)
    {
      addr.m_len = addrlen;
      return true;
    }
    return false;
  }
 
  bool listen(int backlog)
  {
    if (invalid())
    {
      return false;
    }
    return (::listen(m_sock, backlog) == 0);
  }
 
  bool accept(Socket &csock, SocketAddr &caddr)
  {
    if (invalid())
    {
      return false;
    }
    Type accepted = Invalid;
    socklen_t addrlen;
    do
    {
      addrlen  = sizeof(caddr.m_data);
      accepted = ::accept(m_sock, caddr, &addrlen);
    } while (accepted == Invalid && interrupted());
 
    csock = Socket(accepted);
    if (!csock.invalid())
    {
      csock.setNoSigPipe();
      caddr.m_len = addrlen;
      return true;
    }
    return false;
  }
 
  bool shutdown(int how)
  {
    if (invalid())
    {
      return false;
    }
    return (::shutdown(m_sock, how) == 0);
  }
 
  static int error()
  {
#ifdef _WIN32
    return ::WSAGetLastError();
#else
    return errno;
#endif
  }
 
  /// True if the error code means "try again later" on a non-blocking socket.
  static bool isWouldBlock(int err)
  {
#ifdef _WIN32
    return err == WSAEWOULDBLOCK;
#else
    return err == EWOULDBLOCK || err == EAGAIN;
#endif
  }
 
  enum
  {
#ifdef _WIN32
    ErrorWouldBlock = WSAEWOULDBLOCK
#else
    ErrorWouldBlock = EWOULDBLOCK
#endif
  };
 
  enum
  {
#ifdef _WIN32
    ShutdownReceive = SD_RECEIVE,
    ShutdownSend    = SD_SEND,
    ShutdownBoth    = SD_BOTH
#else
    ShutdownReceive = SHUT_RD,
    ShutdownSend    = SHUT_WR,
    ShutdownBoth    = SHUT_RDWR
#endif
  };
 
private:
  static IoLen clampLength(unsigned size)
  {
    return static_cast<IoLen>(size > static_cast<unsigned>(INT_MAX) ? static_cast<unsigned>(INT_MAX)
                                                                    : size);
  }
 
  static bool interrupted()
  {
#ifdef _WIN32
    return false;
#else
    return errno == EINTR;
#endif
  }
};
 
/// <summary>
/// Socket Data
/// </summary>
struct SocketData
{
  Socket socket;
  int flags{0};
#ifdef _WIN32
  WSAEVENT event{WSA_INVALID_EVENT};
#endif
 
  bool operator==(const Socket &s) const { return (socket == s); }
};
 
/// <summary>
/// Socket Reactor.
///
/// Semantics (identical on all platforms):
///  - Readable / Writable / Acceptable callbacks are delivered only if the matching flag is set.
///    If both Acceptable and Readable are set, Acceptable wins for read-readiness.
///  - Closed (peer hang-up / error) is ALWAYS delivered, independent of the Closed flag (the flag
///    is kept for API compatibility). The socket is automatically unregistered from the reactor
///    before onSocketClosed() is called; the reactor never closes the socket itself.
///  - Callbacks run on the reactor thread without any internal lock held, so they may freely call
///    addSocket() / removeSocket().
///  - All public methods are thread-safe.
/// </summary>
struct Reactor : protected common::Thread
{
  class SocketCallback
  {
  public:
    SocketCallback() = default;
 
    SocketCallback(const SocketCallback &)            = delete;
    SocketCallback(SocketCallback &&)                 = delete;
    SocketCallback &operator=(const SocketCallback &) = delete;
    SocketCallback &operator=(SocketCallback &&)      = delete;
 
    virtual ~SocketCallback()                    = default;
    virtual void onSocketReadable(Socket sock)   = 0;
    virtual void onSocketWritable(Socket sock)   = 0;
    virtual void onSocketAcceptable(Socket sock) = 0;
    virtual void onSocketClosed(Socket sock)     = 0;
  };
 
  enum State : std::uint8_t
  {
    Readable   = 1,
    Writable   = 2,
    Acceptable = 4,
    Closed     = 8
  };
 
  SocketCallback &m_callback;
  std::vector<SocketData> m_sockets;
 
private:
  std::mutex m_mutex;  // guards m_sockets (and the OS registration that mirrors it)
 
#ifdef __linux__
  int m_epollFd{-1};
#endif
 
#ifdef __APPLE__
  enum
  {
    KqueueBatchSize = 32
  };
  int m_kq{-1};
#endif
 
public:
  explicit Reactor(SocketCallback &callback) : m_callback(callback)
  {
#ifdef __linux__
#  if defined(ANDROID) || defined(__ANDROID__)
    m_epollFd = ::epoll_create(1);
#  else
    m_epollFd = ::epoll_create1(EPOLL_CLOEXEC);
#  endif
    if (m_epollFd == -1)
    {
      LOG_ERROR("Reactor: epoll_create failed, errno=%d", errno);
    }
#endif
#ifdef __APPLE__
    m_kq = ::kqueue();
    if (m_kq == -1)
    {
      LOG_ERROR("Reactor: kqueue failed, errno=%d", errno);
    }
#endif
  }
 
  Reactor(const Reactor &)            = delete;
  Reactor(Reactor &&)                 = delete;
  Reactor &operator=(const Reactor &) = delete;
  Reactor &operator=(Reactor &&)      = delete;
 
  ~Reactor() override
  {
    stop();  // joins the thread (must happen while the derived part is still alive)
#ifdef __linux__
    if (m_epollFd != -1)
    {
      ::close(m_epollFd);
      m_epollFd = -1;
    }
#endif
#ifdef __APPLE__
    if (m_kq != -1)
    {
      ::close(m_kq);
      m_kq = -1;
    }
#endif
  }
 
  /// Registers a socket or updates its flags. flags == 0 removes it.
  /// Returns false if the OS registration failed.
  bool addSocket(const Socket &socket, int flags)
  {
    if (flags == 0)
    {
      removeSocket(socket);
      return true;
    }
    if (socket.invalid())
    {
      return false;
    }
 
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = findLocked(socket);
    if (it == m_sockets.end())
    {
      LOG_TRACE("Reactor: Adding socket 0x%llx with flags 0x%x",
                static_cast<unsigned long long>(socket.m_sock), flags);
      SocketData sd;
      sd.socket = socket;
      sd.flags  = flags;
      if (!registerLocked(sd))
      {
        LOG_ERROR("Reactor: failed to register socket");
        return false;
      }
      m_sockets.push_back(sd);
      return true;
    }
 
    LOG_TRACE("Reactor: Updating socket 0x%llx with flags 0x%x",
              static_cast<unsigned long long>(socket.m_sock), flags);
    if (it->flags != flags)
    {
      if (!updateLocked(*it, flags))
      {
        LOG_ERROR("Reactor: failed to update socket");
        return false;
      }
      it->flags = flags;
    }
    return true;
  }
 
  void removeSocket(const Socket &socket)
  {
    LOG_TRACE("Reactor: Removing socket 0x%llx", static_cast<unsigned long long>(socket.m_sock));
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = findLocked(socket);
    if (it != m_sockets.end())
    {
      unregisterLocked(*it);
      m_sockets.erase(it);
    }
  }
 
  void start()
  {
    LOG_INFO("Reactor: Starting...");
    startThread();
  }
 
  void stop()
  {
    LOG_INFO("Reactor: Stopping...");
    joinThread();
 
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto &sd : m_sockets)
    {
      unregisterLocked(sd);
    }
    m_sockets.clear();
  }
 
protected:
  void onThread() override
  {
    LOG_INFO("Reactor: Thread started");
    while (!shouldTerminate())
    {
      pollOnce();
    }
    LOG_TRACE("Reactor: Thread done");
  }
 
private:
  using Iterator = std::vector<SocketData>::iterator;
 
  Iterator findLocked(const Socket &socket)
  {
    return std::find_if(m_sockets.begin(), m_sockets.end(),
                        [&socket](const SocketData &sd) { return sd.socket == socket; });
  }
 
  bool getFlags(const Socket &socket, int &flags)
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = findLocked(socket);
    if (it == m_sockets.end())
    {
      return false;
    }
    flags = it->flags;
    return true;
  }
 
  void dispatchClosed(const Socket &socket)
  {
    removeSocket(socket);
    m_callback.onSocketClosed(socket);
  }
 
  // ---------------------------------------------------------------- Windows
#ifdef _WIN32
  static long toNetworkEvents(int flags)
  {
    long events = FD_CLOSE;  // close is always reported
    if (flags & Readable)   events |= FD_READ;
    if (flags & Writable)   events |= FD_WRITE;
    if (flags & Acceptable) events |= FD_ACCEPT;
    return events;
  }
 
  bool registerLocked(SocketData &sd)
  {
    if (m_sockets.size() >= WSA_MAXIMUM_WAIT_EVENTS)
    {
      LOG_ERROR("Reactor: too many sockets (max %d)", static_cast<int>(WSA_MAXIMUM_WAIT_EVENTS));
      return false;
    }
    WSAEVENT ev = ::WSACreateEvent();
    if (ev == WSA_INVALID_EVENT)
    {
      return false;
    }
    if (::WSAEventSelect(sd.socket, ev, toNetworkEvents(sd.flags)) == SOCKET_ERROR)
    {
      ::WSACloseEvent(ev);
      return false;
    }
    sd.event = ev;
    return true;
  }
 
  bool updateLocked(SocketData &sd, int flags)
  {
    return ::WSAEventSelect(sd.socket, sd.event, toNetworkEvents(flags)) != SOCKET_ERROR;
  }
 
  void unregisterLocked(SocketData &sd)
  {
    if (sd.event != WSA_INVALID_EVENT)
    {
      ::WSAEventSelect(sd.socket, sd.event, 0);
      ::WSACloseEvent(sd.event);
      sd.event = WSA_INVALID_EVENT;
    }
  }
 
  void pollOnce()
  {
    std::vector<SocketData> snapshot;
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      snapshot = m_sockets;
    }
    if (snapshot.empty())
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      return;
    }
 
    std::vector<WSAEVENT> events;
    events.reserve(snapshot.size());
    for (const auto &sd : snapshot)
    {
      events.push_back(sd.event);
    }
 
    // Only the lowest signaled index is returned, so afterwards every socket is polled.
    DWORD result = ::WSAWaitForMultipleEvents(static_cast<DWORD>(events.size()), events.data(),
                                              FALSE, 500, FALSE);
    if (result == WSA_WAIT_TIMEOUT)
    {
      return;
    }
    if (result == WSA_WAIT_FAILED)
    {
      // e.g. an event was closed by removeSocket() after the snapshot; retry with a fresh one
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      return;
    }
 
    for (const auto &sd : snapshot)
    {
      if (shouldTerminate())
      {
        return;
      }
      WSANETWORKEVENTS ne{};
      if (::WSAEnumNetworkEvents(sd.socket, sd.event, &ne) != 0 || ne.lNetworkEvents == 0)
      {
        continue;
      }
 
      int flags = 0;
      if (!getFlags(sd.socket, flags))
      {
        continue;
      }
 
      if ((flags & Readable) && (ne.lNetworkEvents & FD_READ))
        m_callback.onSocketReadable(sd.socket);
      if ((flags & Writable) && (ne.lNetworkEvents & FD_WRITE))
        m_callback.onSocketWritable(sd.socket);
      if ((flags & Acceptable) && (ne.lNetworkEvents & FD_ACCEPT))
        m_callback.onSocketAcceptable(sd.socket);
      if (ne.lNetworkEvents & FD_CLOSE)
        dispatchClosed(sd.socket);
    }
  }
#endif  // _WIN32
 
  // ------------------------------------------------------------------ Linux
#ifdef __linux__
  static uint32_t toEpollEvents(int flags)
  {
    uint32_t events = 0;  // EPOLLHUP / EPOLLERR are always reported by the kernel
    if (flags & (Readable | Acceptable)) events |= EPOLLIN;
    if (flags & Writable)                events |= EPOLLOUT;
    return events;
  }
 
  bool registerLocked(SocketData &sd)
  {
    if (m_epollFd == -1)
    {
      return false;
    }
    epoll_event event = {};
    event.data.fd     = sd.socket;
    event.events      = toEpollEvents(sd.flags);
    return ::epoll_ctl(m_epollFd, EPOLL_CTL_ADD, sd.socket, &event) == 0;
  }
 
  bool updateLocked(SocketData &sd, int flags)
  {
    if (m_epollFd == -1)
    {
      return false;
    }
    epoll_event event = {};
    event.data.fd     = sd.socket;
    event.events      = toEpollEvents(flags);
    return ::epoll_ctl(m_epollFd, EPOLL_CTL_MOD, sd.socket, &event) == 0;
  }
 
  void unregisterLocked(SocketData &sd)
  {
    if (m_epollFd != -1)
    {
      ::epoll_ctl(m_epollFd, EPOLL_CTL_DEL, sd.socket, nullptr);
    }
  }
 
  void pollOnce()
  {
    epoll_event events[64];
    int result = ::epoll_wait(m_epollFd, events, static_cast<int>(sizeof(events) / sizeof(events[0])),
                              500);
    if (result < 0)
    {
      if (errno != EINTR)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));  // avoid a busy loop
      }
      return;
    }
 
    for (int i = 0; i < result; i++)
    {
      Socket socket(events[i].data.fd);
      int flags = 0;
      if (!getFlags(socket, flags))
      {
        continue;
      }
      const uint32_t ev = events[i].events;
 
      if (flags & Acceptable)
      {
        if (ev & EPOLLIN)
          m_callback.onSocketAcceptable(socket);
      }
      else if ((flags & Readable) && (ev & EPOLLIN))
      {
        m_callback.onSocketReadable(socket);
      }
      if ((flags & Writable) && (ev & EPOLLOUT))
        m_callback.onSocketWritable(socket);
      if (ev & (EPOLLHUP | EPOLLERR))
        dispatchClosed(socket);
    }
  }
#endif  // __linux__
 
  // ------------------------------------------------------------------ macOS
#ifdef __APPLE__
  void kqSet(int fd, int16_t filter, uint16_t kflags)
  {
    struct kevent event;
    EV_SET(&event, static_cast<uintptr_t>(fd), filter, kflags, 0, 0, nullptr);
    ::kevent(m_kq, &event, 1, nullptr, 0, nullptr);  // EV_DELETE on a missing filter is harmless
  }
 
  // Brings the registered filters from oldFlags to newFlags.
  void kqUpdate(int fd, int oldFlags, int newFlags)
  {
    const int readMask  = Readable | Acceptable;
    const bool oldRead  = (oldFlags & (readMask | Closed)) != 0;
    const bool newRead  = (newFlags & (readMask | Closed)) != 0;
    const bool oldWrite = (oldFlags & Writable) != 0;
    const bool newWrite = (newFlags & Writable) != 0;
 
    if (newRead)
    {
      // Closed-only sockets use EV_CLEAR so unread data does not cause endless wake-ups.
      const uint16_t extra = (newFlags & readMask) ? 0 : EV_CLEAR;
      kqSet(fd, EVFILT_READ, static_cast<uint16_t>(EV_ADD | extra));
    }
    else if (oldRead)
    {
      kqSet(fd, EVFILT_READ, EV_DELETE);
    }
 
    if (newWrite)
    {
      kqSet(fd, EVFILT_WRITE, EV_ADD);
    }
    else if (oldWrite)
    {
      kqSet(fd, EVFILT_WRITE, EV_DELETE);
    }
  }
 
  bool registerLocked(SocketData &sd)
  {
    if (m_kq == -1)
    {
      return false;
    }
    kqUpdate(sd.socket, 0, sd.flags);
    return true;
  }
 
  bool updateLocked(SocketData &sd, int flags)
  {
    if (m_kq == -1)
    {
      return false;
    }
    kqUpdate(sd.socket, sd.flags, flags);
    return true;
  }
 
  void unregisterLocked(SocketData &sd)
  {
    if (m_kq != -1)
    {
      kqUpdate(sd.socket, sd.flags, 0);
    }
  }
 
  void pollOnce()
  {
    struct kevent events[KqueueBatchSize];
    struct timespec timeout;
    timeout.tv_sec  = 0;
    timeout.tv_nsec = 500 * 1000 * 1000;
 
    int nev = ::kevent(m_kq, nullptr, 0, events, KqueueBatchSize, &timeout);
    if (nev < 0)
    {
      if (errno != EINTR)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
      return;
    }
 
    for (int i = 0; i < nev; i++)
    {
      const struct kevent &event = events[i];
      Socket socket(static_cast<int>(event.ident));
      int flags = 0;
      if (!getFlags(socket, flags))
      {
        continue;
      }
 
      // Errors and EOF are checked first, so they can never be swallowed by a filter branch.
      if (event.flags & EV_ERROR)
      {
        dispatchClosed(socket);
        continue;
      }
 
      if (event.filter == EVFILT_READ)
      {
        if (flags & Acceptable)
        {
          if (event.data > 0)
            m_callback.onSocketAcceptable(socket);
        }
        else if ((flags & Readable) && event.data > 0)
        {
          m_callback.onSocketReadable(socket);  // deliver pending data before reporting EOF
        }
      }
      else if (event.filter == EVFILT_WRITE)
      {
        if ((flags & Writable) && !(event.flags & EV_EOF))
          m_callback.onSocketWritable(socket);
      }
 
      if (event.flags & EV_EOF)
      {
        dispatchClosed(socket);
      }
    }
  }
#endif  // __APPLE__
};
 
}  // namespace SocketTools
 
