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

#pragma once

#include <chrono>
#include <codecvt>
#include <ctime>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>

#include "opentelemetry/exporters/etw/uuid.hpp"
#include "opentelemetry/version.h"

#ifdef _WIN32
#  include <Windows.h>
#  include <evntprov.h>
#  include <wincrypt.h>
#  pragma comment(lib, "Advapi32.lib")
#  pragma comment(lib, "Rpcrt4.lib")
#endif

OPENTELEMETRY_BEGIN_NAMESPACE

namespace utils
{

/// <summary>
/// Compile-time constexpr djb2 hash function for strings
/// </summary>
static constexpr uint32_t hashCode(const char *str, uint32_t h = 0)
{
  return (uint32_t)(!str[h] ? 5381 : ((uint32_t)hashCode(str, h + 1) * (uint32_t)33) ^ str[h]);
}

#define CONST_UINT32_T(x) std::integral_constant<uint32_t, (uint32_t)x>::value

#define CONST_HASHCODE(name) CONST_UINT32_T(OPENTELEMETRY_NAMESPACE::utils::hashCode(#name))

#ifdef _WIN32

/// <summary>
/// Compute SHA-1 hash of input buffer and save to output
/// </summary>
/// <param name="pData">Input buffer</param>
/// <param name="nData">Input buffer size</param>
/// <param name="pHashedData">Output buffer</param>
/// <param name="nHashedData">Output buffer size</param>
/// <returns></returns>
static inline bool sha1(const BYTE *pData, DWORD nData, BYTE *pHashedData, DWORD &nHashedData)
{
  bool bRet        = false;
  HCRYPTPROV hProv = NULL;
  HCRYPTHASH hHash = NULL;

  if (!CryptAcquireContext(&hProv,                // handle of the CSP
                           NULL,                  // key container name
                           NULL,                  // CSP name
                           PROV_RSA_FULL,         // provider type
                           CRYPT_VERIFYCONTEXT))  // no key access is requested
  {
    bRet = false;
    goto CleanUp;
  }

  if (!CryptCreateHash(hProv,      // handle of the CSP
                       CALG_SHA1,  // hash algorithm to use
                       0,          // hash key
                       0,          // reserved
                       &hHash))    //
  {
    bRet = false;
    goto CleanUp;
  }

  if (!CryptHashData(hHash,  // handle of the HMAC hash object
                     pData,  // message to hash
                     nData,  // number of bytes of data to add
                     0))     // flags
  {
    bRet = false;
    goto CleanUp;
  }

  if (!CryptGetHashParam(hHash,         // handle of the HMAC hash object
                         HP_HASHVAL,    // query on the hash value
                         pHashedData,   // filled on second call
                         &nHashedData,  // length, in bytes,of the hash
                         0))
  {
    bRet = false;
    goto CleanUp;
  }

  bRet = true;

CleanUp:

  if (hHash)
  {
    CryptDestroyHash(hHash);
  }

  if (hProv)
  {
    CryptReleaseContext(hProv, 0);
  }
  return bRet;
}

/// <summary>
/// Convert UTF-8 string to UTF-16 wide string.
///
/// FIXME: this conversion is marked deprecated after C++17:
/// https://en.cppreference.com/w/cpp/locale/codecvt_utf8_utf16
/// It works well with Visual C++, but may not work with clang.
/// Best long-term solution is to use Win32 API instead.
///
/// </summary>
/// <param name="in"></param>
/// <returns></returns>
static inline std::wstring to_utf16_string(const std::string &in)
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
  return converter.from_bytes(in);
}

/// <summary>
/// Transform ETW provider name to provider GUID as described here:
/// https://blogs.msdn.microsoft.com/dcook/2015/09/08/etw-provider-names-and-guids/
/// </summary>
/// <param name="providerName"></param>
/// <returns></returns>
static inline GUID GetProviderGuid(const char *providerName)
{
  std::string name(providerName);
  std::transform(name.begin(), name.end(), name.begin(),
                 [](unsigned char c) { return (char)::toupper(c); });

  size_t len      = name.length() * 2 + 0x10;
  uint8_t *buffer = new uint8_t[len];
  uint32_t num    = 0x482c2db2;
  uint32_t num2   = 0xc39047c8;
  uint32_t num3   = 0x87f81a15;
  uint32_t num4   = 0xbfc130fb;

  for (int i = 3; i >= 0; i--)
  {
    buffer[i]      = (uint8_t)num;
    num            = num >> 8;
    buffer[i + 4]  = (uint8_t)num2;
    num2           = num2 >> 8;
    buffer[i + 8]  = (uint8_t)num3;
    num3           = num3 >> 8;
    buffer[i + 12] = (uint8_t)num4;
    num4           = num4 >> 8;
  }

  for (size_t j = 0; j < name.length(); j++)
  {
    buffer[((2 * j) + 0x10) + 1] = (uint8_t)name[j];
    buffer[(2 * j) + 0x10]       = (uint8_t)(name[j] >> 8);
  }

  const size_t sha1_hash_size = 21;
  uint8_t *buffer2            = new uint8_t[sha1_hash_size];
  DWORD len2                  = sha1_hash_size;
  sha1((const BYTE *)buffer, (DWORD)len, (BYTE *)buffer2, len2);

  unsigned long a     = (((((buffer2[3] << 8) + buffer2[2]) << 8) + buffer2[1]) << 8) + buffer2[0];
  unsigned short b    = (unsigned short)((buffer2[5] << 8) + buffer2[4]);
  unsigned short num9 = (unsigned short)((buffer2[7] << 8) + buffer2[6]);

  GUID guid;
  guid.Data1    = a;
  guid.Data2    = b;
  guid.Data3    = (unsigned short)((num9 & 0xfff) | 0x5000);
  guid.Data4[0] = buffer2[8];
  guid.Data4[1] = buffer2[9];
  guid.Data4[2] = buffer2[10];
  guid.Data4[3] = buffer2[11];
  guid.Data4[4] = buffer2[12];
  guid.Data4[5] = buffer2[13];
  guid.Data4[6] = buffer2[14];
  guid.Data4[7] = buffer2[15];

  delete buffer;
  delete buffer2;

  return guid;
}
#endif

};  // namespace utils

OPENTELEMETRY_END_NAMESPACE
