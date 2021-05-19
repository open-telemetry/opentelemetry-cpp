// Copyright 2020, OpenTelemetry Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "udp_transport.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace jaeger
{

UDPTransport::UDPTransport(const std::string &addr, uint16_t port)
    : max_packet_size_(kUDPPacketMaxLength)
{
  InitSocket();

  endpoint_transport_ = std::shared_ptr<TTransport>(new TUDPTransport(addr, port));
  endpoint_transport_->open();
  transport_ = std::shared_ptr<TTransport>(new TBufferedTransport(endpoint_transport_));
  protocol_  = std::shared_ptr<TProtocol>(new TCompactProtocol(transport_));
  agent_     = std::unique_ptr<AgentClient>(new AgentClient(protocol_));
}

UDPTransport::~UDPTransport()
{
  CleanSocket();
}

void UDPTransport::InitSocket()
{
#if defined(_WIN32)
  /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
  WORD wVersionRequested = MAKEWORD(2, 2);

  WSADATA wsaData;
  int err = WSAStartup(wVersionRequested, &wsaData);
  if (err != 0)
  {
    // TODO: handle error
    return;
  }

  /* Confirm that the WinSock DLL supports 2.2.*/
  /* Note that if the DLL supports versions greater    */
  /* than 2.2 in addition to 2.2, it will still return */
  /* 2.2 in wVersion since that is the version we      */
  /* requested.                                        */

  if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
  {
    // TODO: handle error that WinSock 2.2 is not supported.
    WSACleanup();

    return;
  }
#endif
}

void UDPTransport::CleanSocket()
{
#if defined(_WIN32)
  WSACleanup();
#endif
}

void UDPTransport::EmitBatch(const thrift::Batch &batch)
{
  try
  {
    agent_->emitBatch(batch);
  }
  catch (...)
  {}
}

}  // namespace jaeger
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
