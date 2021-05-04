// Copyright 2021, OpenTelemetry Authors
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
#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

/**
 * Socket server intended to be used for local loopback test purposes.
 * Default namespace is: "testing". You can override it using:
 * `#define SOCKET_SERVER_NS alternate_namespace_to_use`
 */
#ifndef SOCKET_SERVER_NS
#  define SOCKET_SERVER_NS testing
#endif

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <thread>

#include "opentelemetry/ext/http/common/macros.h"
#include "socket_tools.h"

using namespace SocketTools;

namespace SOCKET_SERVER_NS
{

/**
 * @brief Common Server for TCP, UDP and Unix Domain.
 */
struct SocketServer : public Reactor::SocketCallback
{
  struct Connection
  {
    enum State
    {
      Idle,        // No data transfer initiated
      Receiving,   // Receiving data
      Responding,  // Sending data
      Closing,     // Closing connection
      Closed,      // Closed connection
      Aborted      // Connection aborted
    };

    Socket socket;      // Active client-server socket
    SocketAddr client;  // Client address

    std::string buffer_received;  // Receive buffer for current event
    size_t bytes_received;        // Total bytes size from client

    std::string buffer_sent;  // Send buffer for current event
    size_t bytes_sent;        // Total bytes bytes_sent to client

    std::set<State> state;  // Current connection state
    bool keepalive;         // Keep connection alive (reserved for future use)
  };

  SocketAddr bind_address;            // Server bind address
  SocketParams server_socket_params;  // Server socket params
  Socket server_socket;               // Server listening socket
  Reactor reactor;                    // Socket event handler

  // Custom callback when server receives a buffer
  std::function<void(Connection &conn)> onRequest;

  // Custom callback when server needs to send a response
  std::function<void(Connection &conn)> onResponse;

  // Active client-server connections protected by recursive mutex
  std::recursive_mutex connections_mutex;
  std::map<SocketTools::Socket, Connection> connections;

  // Macro to safely obtain TEMPORARY string buffer pointer
#define CLID(conn) conn.client.toString().c_str()

  const SocketAddr &address() const { return bind_address; };

  /**
   * @brief Route to start TCP, UDP or Unix Domain socket server.
   * @param addr Address or Unix domain socket name to bind to.
   * @param sock Socket type.
   * @param numConnections Maximum number of connections.
   */
  SocketServer(SocketAddr addr, SocketParams params, int numConnections = 10)
      : bind_address(addr), server_socket_params(params), reactor(*this)
  {
    server_socket = Socket(server_socket_params);

    // Default lambda here implements an echo server
    onRequest = [this](Connection &conn) {
      conn.buffer_sent.clear();
      conn.buffer_received.swap(conn.buffer_sent);
      conn.state.insert(SocketServer::Connection::Responding);
    };
    onResponse = [this](Connection &) {};

    int rc = server_socket.bind(bind_address);
    if (rc != 0)
    {
      LOG_ERROR("Server: bind failed! result=%d", rc);
      return;
    }

    LOG_INFO("Server: bind successful. result=%d", rc);
    server_socket.getsockname(bind_address);
    if (server_socket_params.type == SOCK_STREAM)
    {
      // In TCP and Unix Domain mode we listen and accept.
      reactor.addSocket(server_socket, Reactor::Acceptable);
      server_socket.listen(numConnections);
    }
    else
    {
      // In UDP mode we read in a loop, no need to accept.
      reactor.addSocket(server_socket, Reactor::Readable);
    }

    LOG_INFO("Server: Listening on %s://%s", server_socket_params.scheme(),
             bind_address.toString().c_str());
  }

  /**
   * @brief Start server.
   */
  void Start() { reactor.start(); }

  /**
   * @brief Stop server.
   */
  void Stop() { reactor.stop(); }

  /**
   * @brief Handle Reactor::State::Acceptable event.
   * @param socket Client socket.
   */
  virtual void onSocketAcceptable(Socket socket) override
  {
    LOG_TRACE("Server: accepting socket fd=0x%llx", socket.m_sock);

    //
    Socket csocket;
    SocketAddr caddr;

    if (socket.accept(csocket, caddr))
    {
#ifdef HAVE_UNIX_DOMAIN
      // If server is Unix domain, then the client socket is also Unix domain
      if (bind_address.isUnixDomain)
      {
        caddr.isUnixDomain = bind_address.isUnixDomain;
        // Sometimes AF_UNIX does not auto-populate
        // the bind address on accept. Thus, copy.
        std::copy(std::begin(bind_address.m_data_un.sun_path),
                  std::end(bind_address.m_data_un.sun_path), std::begin(caddr.m_data_un.sun_path));
      };
#endif

      LOCKGUARD(connections_mutex);
      csocket.setNonBlocking();
      Connection &conn    = connections[csocket];
      conn.socket         = csocket;
      conn.state          = {Connection::Idle};
      conn.client         = caddr;
      conn.bytes_received = 0;
      conn.bytes_sent     = 0;
      conn.keepalive      = false;
      reactor.addSocket(csocket, Reactor::Readable | Reactor::Closed);
      LOG_TRACE("Server: [%s] accepted", CLID(conn));
    }
  }

  /**
   * @brief Handle Reactor::State::Reasable event.
   * @param socket Client socket.
   */
  virtual void onSocketReadable(Socket socket) override
  {
    LOG_TRACE("Server: reading socket fd=0x%x", static_cast<int>(socket.m_sock));

    LOCKGUARD(connections_mutex);
    int size = 0;
    auto it  = connections.find(socket);
    if (it != connections.end())
    {
      // TCP or Unix domain server
      Connection &conn = it->second;
      // Read 2KB at a time
      char buffer[2048] = {0};
      size              = socket.recv(buffer, sizeof(buffer));
      LOG_TRACE("Server: [%s] received %d bytes", CLID(conn), size);
      if (size <= 0)
      {
        // recv() returns 0 only when you request a 0 - byte buffer or
        // the other peer has gracefully disconnected.
        LOG_TRACE("Server: [%s] closing connection...", CLID(conn));
        onConnectionClosed(conn);
        return;
      }
      conn.buffer_received.append(buffer, buffer + size);
      conn.bytes_received += size;
      HandleConnection(conn);
      return;
    }

    //
    // The rest of this function is handling UDP connections only
    //
    {
      // UDP connection: 1 per callback
      Connection conn;
      conn.socket         = socket;
      conn.state          = {Connection::Receiving};
      conn.bytes_received = 0;
      conn.bytes_sent     = 0;
      // Maximum size is 0xffff - (sizeof(IP Header) + sizeof(UDP Header)).
      // Try to read the entire datagram.
      conn.buffer_received.resize(0xffff);
      size = socket.recvfrom((void *)(conn.buffer_received.data()), 0xffff, 0, conn.client);
      if (size > 0)
      {
        LOG_ERROR("Server: [%s] received % bytes", CLID(conn), size);
        conn.buffer_received.resize(size);
        conn.bytes_received = size;
        HandleConnection(conn);
      }
      else
      {
        conn.buffer_received.resize(0);
        conn.bytes_received = 0;
        LOG_ERROR("Server: [%s] failed to receive client datagram", CLID(conn));
      }
    }
  }

  /**
   * @brief Handle a timeslice of sending data back to client. If sending is blocked,
   * but not all data has been sent, then this function returns with an indication
   * that it needs to be called for the same connection again.
   *
   * @param conn Client-server connection.
   *
   * @return true if not all data has been bytes_sent.
   */
  bool SendResponse(Connection &conn)
  {
    if (conn.buffer_sent.empty())
    {
      return false;
    }

    int bytes_sent = 0;

    uint32_t optval = 0;
    conn.socket.getsockopt(SOL_SOCKET, SO_TYPE, optval);
    if (optval == SOCK_DGRAM)
    {
      bytes_sent = conn.socket.sendto(conn.buffer_sent.data(),
                                      static_cast<int>(conn.buffer_sent.size()), 0, conn.client);
      LOG_TRACE("Server: [%s] datagram sent %d", CLID(conn), bytes_sent);
    }
    else
    {
      bytes_sent =
          conn.socket.send(conn.buffer_sent.data(), static_cast<int>(conn.buffer_sent.size()));
      LOG_TRACE("Server: [%s] stream sent %d", CLID(conn), bytes_sent);
      if (bytes_sent < 0 && conn.socket.error() != SocketTools::Socket::ErrorWouldBlock)
      {
        return true;
      }
    }

    conn.bytes_sent += bytes_sent;
    conn.buffer_sent.erase(0, bytes_sent);

    if (!conn.buffer_sent.empty())
    {
      reactor.addSocket(conn.socket, SocketTools::Reactor::Writable | SocketTools::Reactor::Closed);
      return true;
    }

    return false;
  }

  /**
   * @brief Event triggered when server may write data back to client.
   * @param socket Client socket.
   */
  virtual void onSocketWritable(Socket socket) override
  {
    LOG_TRACE("Server: writing socket fd=0x%llx", socket.m_sock);

    LOCKGUARD(connections_mutex);
    auto it = connections.find(socket);
    if (it == connections.end())
    {
      return;
    }

    Connection &conn = it->second;
    if (!SendResponse(conn))
    {
      HandleConnection(conn);
    }
  }

  /**
   * @brief Handle event when socket is closed.
   * @param socket
   */
  virtual void onSocketClosed(Socket socket) override
  {
    LOG_TRACE("Server: closing socket fd=0x%llx", socket.m_sock);

    LOCKGUARD(connections_mutex);
    auto connIt = connections.find(socket);
    if (connIt == connections.end())
    {
      return;
    }

    Connection &conn = connIt->second;
    onConnectionClosed(conn);
  }

  /**
   * @brief Handle event when connection is closed.
   * @param conn
   */
  void onConnectionClosed(Connection &conn)
  {
    LOG_TRACE("Server: [%s] closed", CLID(conn));
    if ((!conn.state.count(Connection::Idle)) && (!conn.state.count(Connection::Closing)))
    {
      conn.state = {Connection::Aborted};
      onConnectionAborted(conn);
    }
    reactor.removeSocket(conn.socket);

    LOCKGUARD(connections_mutex);
    auto connIt = connections.find(conn.socket);
    conn.socket.close();
    connections.erase(connIt);
  }

  void onConnectionAborted(Connection &conn)
  {
    LOG_WARN("Server: [%s] connection closed unexpectedly", CLID(conn));
  }

  /**
   * @brief Handle connection state update.
   *
   * Connection states:
   * - Idle             - start receiving.
   * - Receiving        - handle client request.
   * - Responding       - respond back to client.
   * - Closing          - closing connection.
   * - Closed           - connection closed.
   *
   * @param conn
   */
  void HandleConnection(Connection &conn)
  {

    if (conn.state.count(Connection::Idle))
    {
      conn.state = {Connection::Receiving};
      LOG_TRACE("Server: [%s] Idle -> Receiving", CLID(conn));
    }

    if (conn.state.count(Connection::Receiving))
    {
      // Got data to process
      if (!conn.buffer_received.empty())
      {
        LOG_TRACE("Server: [%s] receiving...", CLID(conn));
        onRequest(conn);
      }
    }

    if (conn.state.count(Connection::Responding))
    {
      // Got data to send back
      LOG_TRACE("Server: [%s] responding...", CLID(conn));
      onResponse(conn);
      // If sendResponse returns true, then more data needs to be bytes_sent in the next timeslice
      if (SendResponse(conn))
      {
        return;
      }

      // Done Sending
      conn.state.erase(Connection::Responding);

      // If we are done responding, we may need to keep the socket open
      if (conn.keepalive)
      {
        reactor.addSocket(conn.socket,
                          SocketTools::Reactor::Readable | SocketTools::Reactor::Closed);
        LOG_TRACE("Server: [%s] idle (keep-alive)", CLID(conn));
      }
      else
      {
        LOG_TRACE("Server: [%s] closing connection...", CLID(conn));
        conn.state.insert(Connection::Closing);
      }
    }

    if (conn.state.count(Connection::Closing))
    {
      conn.socket.shutdown(SocketTools::Socket::ShutdownSend);
      reactor.addSocket(conn.socket, SocketTools::Reactor::Closed);
      conn.state.insert(Connection::Closed);
    }
  }
};
}  // namespace SOCKET_SERVER_NS
#endif
