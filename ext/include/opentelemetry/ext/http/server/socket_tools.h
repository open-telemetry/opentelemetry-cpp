// Copyright 2020, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#ifdef _MSC_VER
#  pragma warning(push)
// Disable: warning C4267: 'argument': conversion from `size_t` to `int`, possible loss of data.
//
// WinSock vs POSIX sockets use different definition of socket payload size, e.g.
// in definition of socket ::send 'len' argument:
// - WinSock: int len
// - Linux:   size_t len
// - BSD:     size_t len
//
// We keep C++ method signature identical and prefer `size_t`. It is expected that Windows
// client cann not physically attempt to send a buffer larger than a size of max int.
//
#  pragma warning(disable : 4267)
#endif

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <WS2tcpip.h>
#  include <WinSock2.h>
#  include <Windows.h>

#  ifdef min
// NOMINMAX may be a better choice. However, defining it globally may break other.
// Code that depends on macro definition in Windows SDK.
#    undef min
#    undef max
#  endif

// This code requires WinSock2 on Windows.
#  pragma comment(lib, "ws2_32.lib")
// Workaround for libcurl redefinition of afunix.h struct :
// https://github.com/curl/curl/blob/7645324072c2f052fa662aded6f26821141ecda1/lib/config-win32.h#L721
// Unfortunately libcurl defines a structure that should otherwise be normally defined by afunix.h .
// When that happens, we cannot build the sockets library with Unix domain support.
#  if !defined(USE_UNIX_SOCKETS)
#    ifdef __has_include
#      if __has_include(<afunix.h>)
// Win 10 SDK 17063+ is necessary for Unix domain sockets support.
#        include <afunix.h>
#        define HAVE_UNIX_DOMAIN
#      endif
#    endif
#  endif

#else
#  define HAVE_UNIX_DOMAIN
#  include <unistd.h>

#  ifdef __linux__
#    include <sys/epoll.h>
#  endif

#  if __APPLE__
#    include "TargetConditionals.h"
// Use kqueue on mac
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
#  include <sys/un.h>

#endif

#if !defined(_MSC_VER) && !defined(__STDC_LIB_EXT1__)
#  ifndef strncpy_s
#    define strncpy_s(dest, destsz, src, count) \
      strncpy(dest, src, (destsz <= count) ? destsz : count)
#  endif
#endif

#include "opentelemetry/ext/http/common/macros.h"

namespace common
{

/// <summary>
/// A simple thread, derived class overloads onThread() method.
/// </summary>
struct Thread
{
  std::thread m_thread;

  std::atomic<bool> m_terminate{false};

  /// <summary>
  /// Thread Constructor
  /// </summary>
  /// <returns>Thread</returns>
  Thread() {}

  /// <summary>
  /// Start Thread
  /// </summary>
  void startThread(bool wait = true)
  {
    m_terminate = false;
    m_thread    = std::thread([&]() { this->onThread(); });
    if (wait)
    {
      waitForStart();
    }
  }

  /// <summary>
  /// Wait for thread start
  /// </summary>
  void waitForStart()
  {
    while (!m_thread.joinable())
    {
      std::this_thread::yield();
    }
  }

  /// <summary>
  /// Join Thread
  /// </summary>
  void joinThread()
  {
    m_terminate = true;
    if (m_thread.joinable())
    {
      m_thread.join();
    }
  }

  /// <summary>
  /// Indicates if this thread should terminate
  /// </summary>
  /// <returns></returns>
  bool shouldTerminate() const { return m_terminate; }

  /// <summary>
  /// Must be implemented by children
  /// </summary>
  virtual void onThread() = 0;

  /// <summary>
  /// Thread destructor
  /// </summary>
  /// <returns></returns>
  virtual ~Thread() noexcept {}
};

};  // namespace common
namespace SocketTools
{

#ifdef _WIN32
// WinSocks need extra (de)initialization, solved by a global object here,
// whose constructor/destructor will be called before and after main().
struct WsaInitializer
{
  WsaInitializer()
  {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
  }

  ~WsaInitializer() { WSACleanup(); }
};

static WsaInitializer g_wsaInitializer;

#endif

/// <summary>
/// Encapsulation of C struct sockaddr[_in|_un] with additional helper methods.
/// Both Internet and Unix Domain sockets are suported.
/// The struct may be cast directly to sockaddr:
/// - operator sockaddr *()
/// - size_t size() const - returns proper size depending on socket type.
/// </summary>
struct SocketAddr
{
  // The union is only as big as necessary to hold its largest data member.
  // Modern OS (both Un*x and Windows) require at least sizeof(sockaddr_un).
  union
  {
    sockaddr m_data;
    sockaddr_in m_data_in;
    sockaddr_in6 m_data_in6;
#ifdef HAVE_UNIX_DOMAIN
    sockaddr_un m_data_un;
#endif
  };

  constexpr static u_long const Loopback = 0x7F000001;

  // Indicator that the sockaddr is sockaddr_un
  bool isUnixDomain;

  /// <summary>
  /// SocketAddr constructor
  /// </summary>
  /// <returns>SocketAddr</returns>
  SocketAddr()
  {
    isUnixDomain = false;
#ifdef HAVE_UNIX_DOMAIN
    memset(&m_data_un, 0, sizeof(m_data_un));
#else
    memset(&m_data_in6, 0, sizeof(m_data_in6));
#endif
  }

  SocketAddr(u_long addr, int port)
  {
    isUnixDomain          = false;
    sockaddr_in &inet4    = reinterpret_cast<sockaddr_in &>(m_data);
    inet4.sin_family      = AF_INET;
    inet4.sin_port        = htons(static_cast<unsigned short>(port));
    inet4.sin_addr.s_addr = htonl(addr);
  }

  SocketAddr(char const *addr, bool unixDomain = false) : SocketAddr()
  {
    isUnixDomain = unixDomain;

#ifdef HAVE_UNIX_DOMAIN
    if (isUnixDomain)
    {
      m_data_un.sun_family = AF_UNIX;
      // Max length of Unix domain filename is up to 108 chars
      strncpy_s(m_data_un.sun_path, sizeof(m_data_un.sun_path), addr, sizeof(m_data_un.sun_path));
      return;
    }
#endif

    // Convert {IPv4|IPv6}:{port} string to Network address and Port.
    int port              = 0;
    std::string ipAddress = addr;
    // If numColons is more than 2, then it is IPv6 address
    size_t numColons = std::count(ipAddress.begin(), ipAddress.end(), ':');
    // Find last colon, which should indicate the port number
    char const *lastColon = strrchr(addr, ':');
    if (lastColon)
    {
      port = atoi(lastColon + 1);
      // Erase port number
      ipAddress.erase(lastColon - addr);
    }
    // If there are more than two colons, it means the input is IPv4, e.g
    // [fe80::c018:4a9b:3681:4e41]:3000
    if (numColons > 1)
    {
      sockaddr_in6 &inet6 = m_data_in6;
      inet6.sin6_family   = AF_INET6;
      inet6.sin6_port     = htons(port);
      void *pAddrBuf      = &inet6.sin6_addr;
      size_t len          = ipAddress.length();
      if ((ipAddress[0] == '[') && (ipAddress[len - 1] == ']'))
      {
        // Remove square brackets
        ipAddress = ipAddress.substr(1, ipAddress.length() - 2);
      }
      if (!::inet_pton(inet6.sin6_family, ipAddress.c_str(), pAddrBuf))
      {
        LOG_ERROR("Invalid IPv6 address: %s", addr);
      }
    }
    else
    {
      sockaddr_in &inet = m_data_in;
      inet.sin_family   = AF_INET;
      inet.sin_port     = htons(port);
      void *pAddrBuf    = &inet.sin_addr;
      if (!::inet_pton(inet.sin_family, ipAddress.c_str(), pAddrBuf))
      {
        LOG_ERROR("Invalid IPv4 address: %s", addr);
      }
    }
  }

  SocketAddr(SocketAddr const &other) = default;

  SocketAddr &operator=(SocketAddr const &other) = default;

  operator sockaddr *() { return &m_data; }

  operator const sockaddr *() const { return &m_data; }

  size_t size() const
  {
#ifdef HAVE_UNIX_DOMAIN
    // Unix domain struct m_data_un
    if (isUnixDomain)
      return sizeof(m_data_un);
#endif
    // IPv4 struct m_data_in
    if (m_data.sa_family == AF_INET)
      return sizeof(m_data_in);
    // IPv6 struct m_data_in6
    if (m_data.sa_family == AF_INET6)
      return sizeof(m_data_in6);
    // RAW socket?
    return sizeof(m_data);
  }

  int port() const
  {
#ifdef HAVE_UNIX_DOMAIN
    if (isUnixDomain)
    {
      return -1;
    }
#endif

    switch (m_data.sa_family)
    {
      case AF_INET6: {
        return ntohs(m_data_in6.sin6_port);
      }
      case AF_INET: {
        return ntohs(m_data_in.sin_port);
      }
      default:
        return -1;
    }
  }

  std::string toString() const
  {
    std::ostringstream os;

#ifdef HAVE_UNIX_DOMAIN
    if (isUnixDomain)
    {
      os << (const char *)(m_data_un.sun_path);
    }
    else
#endif
    {
      switch (m_data.sa_family)
      {
        case AF_INET6: {
          char buff[NI_MAXHOST] = {0};
          inet_ntop(AF_INET6, &(m_data_in6.sin6_addr), buff, sizeof(buff));
          os << '[' << buff << ']';
          os << ':' << ntohs(m_data_in6.sin6_port);
          break;
        }
        case AF_INET: {
          u_long addr = ntohl(m_data_in.sin_addr.s_addr);
          os << (addr >> 24) << '.' << ((addr >> 16) & 255) << '.' << ((addr >> 8) & 255) << '.'
             << (addr & 255);
          os << ':' << ntohs(m_data_in.sin_port);
          break;
        }
        default:
          os << "[?AF?" << m_data.sa_family << ']';
      }
    };
    return os.str();
  }
};  // namespace SocketTools

static const char *kSchemeUDP  = "udp";
static const char *kSchemeTCP  = "tcp";
static const char *kSchemeUnix = "unix";
static const char *kSchemeUnk  = "unknown";

struct SocketParams
{
  int af;     // POSIX socket domain
  int type;   // POSIX socket type
  int proto;  // POSIX socket protocol

  /**
   * @brief Determine connection scheme based on socket parameters:
   * "tcp", "udp", "unix" or "unknown".
   *
   * @return Text representation of scheme.
   */
  inline const char *scheme()
  {
    if ((af == AF_INET) || (af == AF_INET6))
    {
      if (type == SOCK_DGRAM)
        return kSchemeUDP;
      if (type == SOCK_STREAM)
        return kSchemeTCP;
    }
    if (af == AF_UNIX)
    {
      return kSchemeUnix;
    }
    return kSchemeUnk;
  }
};

/// <summary>
/// Encapsulation of a socket (non-exclusive ownership)
/// </summary>
struct Socket
{
#ifdef _WIN32
  typedef SOCKET Type;
  static constexpr Type const Invalid = INVALID_SOCKET;
#else
  typedef int Type; /* POSIX m_sock type is int */
  static constexpr Type const Invalid = -1;
#endif

  Type m_sock;

  Socket(SocketParams params) : Socket(params.af, params.type, params.proto) {}

  Socket(Type sock = Invalid) : m_sock(sock) {}

  Socket(int af, int type, int proto) { m_sock = ::socket(af, type, proto); }

  ~Socket() {}

  operator Socket::Type() const { return m_sock; }

  bool operator==(Socket const &other) const { return (m_sock == other.m_sock); }

  bool operator!=(Socket const &other) const { return (m_sock != other.m_sock); }

  bool operator<(Socket const &other) const { return (m_sock < other.m_sock); }

  bool invalid() const { return (m_sock == Invalid); }

  void setNonBlocking()
  {
    assert(m_sock != Invalid);
#ifdef _WIN32
    u_long value = 1;
    ::ioctlsocket(m_sock, FIONBIO, &value);
#else
    int flags = ::fcntl(m_sock, F_GETFL, 0);
    ::fcntl(m_sock, F_SETFL, flags | O_NONBLOCK);
#endif
  }

  bool setReuseAddr()
  {
    assert(m_sock != Invalid);
#ifdef _WIN32
    BOOL value = TRUE;
#else
    int value = 1;
#endif
    return (::setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&value),
                         sizeof(value)) == 0);
  }

  bool setNoDelay()
  {
    assert(m_sock != Invalid);
#ifdef _WIN32
    BOOL value = TRUE;
#else
    int value = 1;
#endif
    return (::setsockopt(m_sock, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&value),
                         sizeof(value)) == 0);
  }

  bool connect(SocketAddr const &addr)
  {
    assert(m_sock != Invalid);
    return (::connect(m_sock, (const sockaddr *)addr, addr.size()) == 0);
  }

  void close()
  {
    assert(m_sock != Invalid);
#ifdef _WIN32
    ::closesocket(m_sock);
#else
    ::close(m_sock);
#endif
    m_sock = Invalid;
  }

  int recvfrom(_Out_cap_(size) void *buffer, size_t size, int flags, SocketAddr &clientAddr)
  {
    assert(m_sock != Invalid);
#ifdef _WIN32
    int len = clientAddr.size();
#else
    socklen_t len     = clientAddr.size();
#endif
    return static_cast<int>(
        ::recvfrom(m_sock, reinterpret_cast<char *>(buffer), size, flags, clientAddr, &len));
  }

  int recv(_Out_cap_(size) void *buffer, size_t size, int flags = 0)
  {
    assert(m_sock != Invalid);
    return static_cast<int>(::recv(m_sock, reinterpret_cast<char *>(buffer), size, flags));
  }

  int send(void const *buffer, size_t size)
  {
    assert(m_sock != Invalid);
    return static_cast<int>(::send(m_sock, reinterpret_cast<char const *>(buffer), size, 0));
  }

  int sendto(void const *buffer, size_t size, int flags, SocketAddr &destAddr)
  {
    assert(m_sock != Invalid);
    int len = destAddr.size();
    return static_cast<int>(
        ::sendto(m_sock, reinterpret_cast<char const *>(buffer), size, flags, destAddr, len));
  }

  int bind(SocketAddr const &addr)
  {
    assert(m_sock != Invalid);
    return ::bind(m_sock, addr, addr.size());
  }

  bool getsockname(SocketAddr &addr) const
  {
    assert(m_sock != Invalid);
#ifdef _WIN32
    int addrlen = sizeof(addr);
#else
    socklen_t addrlen = sizeof(addr);
#endif
    return (::getsockname(m_sock, addr, &addrlen) == 0);
  }

  template <typename T>
  int getsockopt(int level, int optname, T &optval)
  {
#ifdef _WIN32
    int optlen = sizeof(T);
    return ::getsockopt(m_sock, level, optname, (char *)(&optval), &optlen);
#else
    socklen_t optlen  = sizeof(T);
    return ::getsockopt(m_sock, level, optname, (void *)&optval, &optlen);
#endif
  }

  bool listen(size_t backlog)
  {
    assert(m_sock != Invalid);
    return (::listen(m_sock, backlog) == 0);
  }

  bool accept(Socket &csock, SocketAddr &caddr)
  {
    assert(m_sock != Invalid);
#ifdef _WIN32
    int addrlen = sizeof(caddr);
#else
    socklen_t addrlen = sizeof(caddr);
#endif
    csock = ::accept(m_sock, caddr, &addrlen);
    return !csock.invalid();
  }

  bool shutdown(int how)
  {
    assert(m_sock != Invalid);
    return (::shutdown(m_sock, how) == 0);
  }

  int error() const
  {
#ifdef _WIN32
    return ::WSAGetLastError();
#else
    return errno;
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
};

/// <summary>
/// Socket Data
/// </summary>
struct SocketData
{
  Socket socket;
  int flags;

  SocketData() : socket(), flags(0) {}

  bool operator==(Socket s) { return (socket == s); }
};

/// <summary>
/// Socket Reactor
/// </summary>
struct Reactor : protected common::Thread
{
  /// <summary>
  /// Socket State callback
  /// </summary>
  class SocketCallback
  {
  public:
    virtual void onSocketReadable(Socket sock)   = 0;
    virtual void onSocketWritable(Socket sock)   = 0;
    virtual void onSocketAcceptable(Socket sock) = 0;
    virtual void onSocketClosed(Socket sock)     = 0;
  };

  /// <summary>
  /// Socket State
  /// </summary>
  enum State
  {
    Readable   = 1,
    Writable   = 2,
    Acceptable = 4,
    Closed     = 8
  };

  SocketCallback &m_callback;

  std::recursive_mutex m_sockets_mutex;
  std::vector<SocketData> m_sockets;

  // Event loop is required for stream sockets
  bool m_streaming{true};

#ifdef _WIN32
  /* use WinSock events on Windows */
  std::vector<WSAEVENT> m_events{};
#endif

#ifdef __linux__
  /* use epoll on Linux */
  int m_epollFd;
#endif

#ifdef TARGET_OS_MAC
  /* use kqueue on Mac */
#  define KQUEUE_SIZE 32
  int kq{0};
  struct kevent m_events[KQUEUE_SIZE];
#endif

public:
  Reactor(SocketCallback &callback) : m_callback(callback)
  {
#ifdef __linux__
#  ifdef ANDROID
    m_epollFd = ::epoll_create(0);
#  else
    m_epollFd = ::epoll_create1(0);
#  endif
#endif

#ifdef TARGET_OS_MAC
    bzero(&m_events[0], sizeof(m_events));
    kq = kqueue();
#endif
  }

  ~Reactor()
  {
#ifdef __linux__
    ::close(m_epollFd);
#endif
#ifdef TARGET_OS_MAC
    ::close(kq);
#endif
  }

  /// <summary>
  /// Add Socket
  /// </summary>
  /// <param name="socket"></param>
  /// <param name="flags"></param>
  void addSocket(const Socket &socket, int flags)
  {
    if (flags == 0)
    {
      removeSocket(socket);
      return;
    }

    LOCKGUARD(m_sockets_mutex);
    if ((flags == State::Readable) && (m_sockets.size() == 0))
    {
      // No listen/accept - readable UDP datagram
      m_streaming = false;
      LOG_TRACE("Reactor: Adding datagram socket 0x%x with flags 0x%x", static_cast<int>(socket),
                flags);
      m_sockets.push_back(SocketData());
      m_sockets.back().socket = socket;
      m_sockets.back().flags  = 0;
      return;
    }

    if (m_streaming)
    {
      auto it = std::find(m_sockets.begin(), m_sockets.end(), socket);
      if (it == m_sockets.end())
      {
        LOG_TRACE("Reactor: Adding socket 0x%x with flags 0x%x", static_cast<int>(socket), flags);
#ifdef _WIN32
        m_events.push_back(::WSACreateEvent());
#endif
#ifdef __linux__
        epoll_event event = {};
        event.data.fd     = socket;
        event.events      = 0;
        ::epoll_ctl(m_epollFd, EPOLL_CTL_ADD, socket, &event);
#endif
#ifdef TARGET_OS_MAC
        struct kevent event;
        bzero(&event, sizeof(event));
        event.ident = socket.m_sock;
        EV_SET(&event, event.ident, EVFILT_READ, EV_ADD, 0, 0, NULL);
        kevent(kq, &event, 1, NULL, 0, NULL);
        EV_SET(&event, event.ident, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
        kevent(kq, &event, 1, NULL, 0, NULL);
#endif
        m_sockets.push_back(SocketData());
        m_sockets.back().socket = socket;
        m_sockets.back().flags  = 0;
        it                      = m_sockets.end() - 1;
      }
      else
      {
        LOG_TRACE("Reactor: Updating socket 0x%x with flags 0x%x", static_cast<int>(socket), flags);
      }

      if (it->flags != flags)
      {
        it->flags = flags;
#ifdef _WIN32
        long lNetworkEvents = 0;
        if (it->flags & Readable)
        {
          lNetworkEvents |= FD_READ;
        }
        if (it->flags & Writable)
        {
          lNetworkEvents |= FD_WRITE;
        }
        if (it->flags & Acceptable)
        {
          lNetworkEvents |= FD_ACCEPT;
        }
        if (it->flags & Closed)
        {
          lNetworkEvents |= FD_CLOSE;
        }
        auto eventIt = m_events.begin() + std::distance(m_sockets.begin(), it);
        ::WSAEventSelect(socket, *eventIt, lNetworkEvents);
#endif
#ifdef __linux__
        int events = 0;
        if (it->flags & Readable)
        {
          events |= EPOLLIN;
        }
        if (it->flags & Writable)
        {
          events |= EPOLLOUT;
        }
        if (it->flags & Acceptable)
        {
          events |= EPOLLIN;
        }
        // if (it->flags & Closed) - always handled (EPOLLERR | EPOLLHUP)
        epoll_event event = {};
        event.data.fd     = socket;
        event.events      = events;
        ::epoll_ctl(m_epollFd, EPOLL_CTL_MOD, socket, &event);
#endif
#ifdef TARGET_OS_MAC
        // TODO: [MG] - Mac OS X socket doesn't currently support updating flags
#endif
      }
    }
  }

  /// <summary>
  /// Remove Socket
  /// </summary>
  /// <param name="socket"></param>
  void removeSocket(const Socket &socket)
  {
    LOCKGUARD(m_sockets_mutex);
    LOG_TRACE("Reactor: Removing socket 0x%x", static_cast<int>(socket));
    auto it = std::find(m_sockets.begin(), m_sockets.end(), socket);
    if (it != m_sockets.end())
    {
      if (m_streaming)
      {
#ifdef _WIN32
        auto eventIt = m_events.begin() + std::distance(m_sockets.begin(), it);
        ::WSAEventSelect(it->socket, *eventIt, 0);
        ::WSACloseEvent(*eventIt);
        m_events.erase(eventIt);
#endif
#ifdef __linux__
        ::epoll_ctl(m_epollFd, EPOLL_CTL_DEL, socket, nullptr);
#endif
#ifdef TARGET_OS_MAC
        struct kevent event;
        bzero(&event, sizeof(event));
        event.ident = socket;
        EV_SET(&event, socket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        if (-1 == kevent(kq, &event, 1, NULL, 0, NULL))
        {
          //// Already removed?
          LOG_ERROR("cannot delete fd=0x%x from kqueue!", event.ident);
        }
        EV_SET(&event, socket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
        if (-1 == kevent(kq, &event, 1, NULL, 0, NULL))
        {
          //// Already removed?
          LOG_ERROR("cannot delete fd=0x%x from kqueue!", event.ident);
        }
#endif
      }
      m_sockets.erase(it);
    }
  }

  /// <summary>
  /// Start server
  /// </summary>
  void start()
  {
    LOG_INFO("Reactor: Starting...");
    startThread();
  }

  /// <summary>
  /// Stop server
  /// </summary>
  void stop()
  {
    LOG_INFO("Reactor: Stopping...");
    // Force-abort the main server socket
    m_sockets[0].socket.close();
    joinThread();

    // Only acquire the lock after the worker(s) have joined
    LOCKGUARD(m_sockets_mutex);
#ifdef _WIN32
    for (auto &hEvent : m_events)
    {
      ::WSACloseEvent(hEvent);
    }
#else /* Linux and Mac */
    for (auto &sd : m_sockets)
    {
#  ifdef __linux__
      ::epoll_ctl(m_epollFd, EPOLL_CTL_DEL, sd.socket, nullptr);
#  endif
#  ifdef TARGET_OS_MAC
      struct kevent event;
      bzero(&event, sizeof(event));
      event.ident = sd.socket;
      EV_SET(&event, sd.socket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
      if (-1 == kevent(kq, &event, 1, NULL, 0, NULL))
      {
        LOG_ERROR("cannot delete fd=0x%x from kqueue!", event.ident);
      }
      EV_SET(&event, sd.socket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
      if (-1 == kevent(kq, &event, 1, NULL, 0, NULL))
      {
        LOG_ERROR("cannot delete fd=0x%x from kqueue!", event.ident);
      }
#  endif
    }
#endif
    m_sockets.clear();
  }

  /// <summary>
  /// Thread Loop for async events processing
  /// </summary>
  virtual void onThread() override
  {
    LOG_INFO("Reactor: Thread started");

    if (!m_streaming)
    {
      // UDP Server implementation.
      // Process only one bound address at the moment, not many.
      // This single-threaded implementation passes UDP buffers
      // to onSocketReadable, that should decide what to do with
      // the socket. Callback may implements its own thread pool.
      LOCKGUARD(m_sockets_mutex);
      Socket &socket = m_sockets[0].socket;
      LOG_TRACE("Reactor: socket 0x%x receive loop started...", static_cast<int>(socket));
      while (!shouldTerminate())
      {
        m_callback.onSocketReadable(socket);
      }
      m_callback.onSocketClosed(socket);
      LOG_TRACE("Reactor: socket 0x%x closed.", static_cast<int>(socket));
      return;
    }

    while (!shouldTerminate())
    {
      // TCP and Unix Domain Server implementation.
      //
      // Use event-based notification with array of client
      // bidirectional stream sockets concurrently processed
      // by single thread. Facility differs depending on OS:
      //
      // - Windows: use WSA socket events
      // - Linux:   use epoll
      // - Mac:     use kqueue
      //
#ifdef _WIN32
      DWORD dwResult = ::WSAWaitForMultipleEvents(static_cast<DWORD>(m_events.size()),
                                                  m_events.data(), FALSE, 500, FALSE);
      if (dwResult == WSA_WAIT_TIMEOUT)
      {
        continue;
      }

      assert(dwResult <= WSA_WAIT_EVENT_0 + m_events.size());
      int index = dwResult - WSA_WAIT_EVENT_0;

      m_sockets_mutex.lock();
      Socket socket = m_sockets[index].socket;
      int flags     = m_sockets[index].flags;
      m_sockets_mutex.unlock();

      WSANETWORKEVENTS ne;
      ::WSAEnumNetworkEvents(socket, m_events[index], &ne);
      LOG_TRACE(
          "Reactor: Handling socket 0x%x (index %d) with active flags 0x%x "
          "(armed 0x%x)",
          static_cast<int>(socket), index, ne.lNetworkEvents, flags);

      if ((flags & Readable) && (ne.lNetworkEvents & FD_READ))
      {
        m_callback.onSocketReadable(socket);
      }
      if ((flags & Writable) && (ne.lNetworkEvents & FD_WRITE))
      {
        m_callback.onSocketWritable(socket);
      }
      if ((flags & Acceptable) && (ne.lNetworkEvents & FD_ACCEPT))
      {
        m_callback.onSocketAcceptable(socket);
      }
      if ((flags & Closed) && (ne.lNetworkEvents & FD_CLOSE))
      {
        m_callback.onSocketClosed(socket);
      }
#endif

#ifdef __linux__
      {
        epoll_event events[4];
        int result = ::epoll_wait(m_epollFd, events, sizeof(events) / sizeof(events[0]), 500);
        if (result == 0 || (result == -1 && errno == EINTR))
        {
          continue;
        }

        assert(result >= 1 && static_cast<size_t>(result) <= sizeof(events) / sizeof(events[0]));

        LOCKGUARD(m_sockets_mutex);
        for (int i = 0; i < result; i++)
        {
          auto it = std::find(m_sockets.begin(), m_sockets.end(), events[i].data.fd);
          assert(it != m_sockets.end());
          Socket socket = it->socket;
          int flags     = it->flags;

          LOG_TRACE("Reactor: Handling socket 0x%x active flags 0x%x (armed 0x%x)",
                    static_cast<int>(socket), events[i].events, flags);

          if ((flags & Readable) && (events[i].events & EPOLLIN))
          {
            m_callback.onSocketReadable(socket);
          }
          if ((flags & Writable) && (events[i].events & EPOLLOUT))
          {
            m_callback.onSocketWritable(socket);
          }
          if ((flags & Acceptable) && (events[i].events & EPOLLIN))
          {
            m_callback.onSocketAcceptable(socket);
          }
          if ((flags & Closed) && (events[i].events & (EPOLLHUP | EPOLLERR)))
          {
            m_callback.onSocketClosed(socket);
          }
        }
      }
#endif

#if defined(TARGET_OS_MAC)
      {
        LOCKGUARD(m_sockets_mutex);
        unsigned waitms = 500;  // never block for more than 500ms
        struct timespec timeout;
        timeout.tv_sec  = waitms / 1000;
        timeout.tv_nsec = (waitms % 1000) * 1000 * 1000;

        int nev = kevent(kq, NULL, 0, m_events, KQUEUE_SIZE, &timeout);
        for (int i = 0; i < nev; i++)
        {
          struct kevent &event = m_events[i];
          int fd               = (int)event.ident;
          auto it              = std::find(m_sockets.begin(), m_sockets.end(), fd);
          assert(it != m_sockets.end());
          Socket socket = it->socket;
          int flags     = it->flags;

          LOG_TRACE("Handling socket 0x%x active flags 0x%x (armed 0x%x)", static_cast<int>(socket),
                    event.flags, event.fflags);

          if (event.filter == EVFILT_READ)
          {
            if (flags & Acceptable)
            {
              m_callback.onSocketAcceptable(socket);
            }
            if (flags & Readable)
            {
              m_callback.onSocketReadable(socket);
            }
            continue;
          }

          if (event.filter == EVFILT_WRITE)
          {
            if (flags & Writable)
            {
              m_callback.onSocketWritable(socket);
            }
            continue;
          }

          if ((event.flags & EV_EOF) || (event.flags & EV_ERROR))
          {
            LOG_TRACE("event.filter=%s", "EVFILT_WRITE");
            m_callback.onSocketClosed(socket);
            it->flags = Closed;
            struct kevent kevt;
            EV_SET(&kevt, event.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
            if (-1 == kevent(kq, &kevt, 1, NULL, 0, NULL))
            {
              LOG_ERROR("cannot delete fd=0x%x from kqueue!", event.ident);
            }
            EV_SET(&kevt, event.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
            if (-1 == kevent(kq, &kevt, 1, NULL, 0, NULL))
            {
              LOG_ERROR("cannot delete fd=0x%x from kqueue!", event.ident);
            }
            continue;
          }
          LOG_ERROR("Reactor: unhandled kevent!");
        }
      }
#endif
    }
    LOG_TRACE("Reactor: Thread done");
  }
};

}  // namespace SocketTools

#ifdef _MSC_VER
#  pragma warning(pop)
#endif
