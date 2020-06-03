#ifdef _WIN32
#  pragma once

// TODO: adjust the code to use MultiByteToWaideChar() and WideCharToMultiByte()
// Currently _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING is defined
// to suppress the codecvt_utf16 warning.

#  include "StreamTracer.hpp"

#  include <codecvt>
#  include <locale>
#  include <string>
#  include <ostream>
#  include <Windows.h>
#  include <evntprov.h>

#  pragma comment(lib, "Advapi32.lib")
#  pragma comment(lib, "Rpcrt4.lib")

OPENTELEMETRY_BEGIN_NAMESPACE

namespace stream
{

/// <summary>
/// Compute SHA-1 hash of input buffer and save to output
/// </summary>
/// <param name="pData">Input buffer</param>
/// <param name="nData">Input buffer size</param>
/// <param name="pHashedData">Output buffer</param>
/// <param name="nHashedData">Output buffer size</param>
/// <returns></returns>
static bool sha1(const BYTE *pData, DWORD nData, BYTE *pHashedData, DWORD &nHashedData)
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
/// Transport layer implementation for ETW.
/// This function converts UTF-8 strings to wide-strings.
/// </summary>
class ETWStringStreamBuffer : public TraceStreamBuffer
{

protected:
  const REGHANDLE INVALID_HANDLE = _UI64_MAX;

  /// <summary>
  /// ETW handle
  /// </summary>
  REGHANDLE handle;

  /// <summary>
  /// Convert UTF-8 string to UTF-8 wide string.
  ///
  /// FIXME: this conversion is marked deprecated after C++17:
  /// https://en.cppreference.com/w/cpp/locale/codecvt_utf8_utf16
  /// It works well with Visual C++, but may not work with clang.
  /// Best long-term solution is to use Win32 API instead.
  ///
  /// </summary>
  /// <param name="in"></param>
  /// <returns></returns>
  inline std::wstring to_utf16_string(const std::string &in)
  {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    return converter.from_bytes(in);
  }

public:
  /// <summary>
  /// Register event provider with ETW by path (string GUID)
  /// </summary>
  /// <param name="ProviderId"></param>
  /// <returns></returns>
  REGHANDLE open(const char *path)
  {
#if 1
    // TODO: validate path
    opentelemetry::event::UUID uuid { path };
    GUID guid = uuid.to_GUID();
#else
    GUID guid;
    if (UuidFromStringA((unsigned char __RPC_FAR *)path, &guid) != RPC_S_OK)
      return INVALID_HANDLE;
#endif
    return open(guid);
  }

  /// <summary>
  /// Register event provider with ETW by GUID
  /// </summary>
  /// <param name="ProviderId"></param>
  /// <returns></returns>
  REGHANDLE open(const GUID &path)
  {
    if (EventRegister(&path, NULL, NULL, &handle) != ERROR_SUCCESS)
    {
      // There was an error registering the ETW provider
      handle = INVALID_HANDLE;
    }
    return handle;
  }

  /// <summary>
  /// Unregister ETW handle
  /// </summary>
  void close()
  {
    if (handle != INVALID_HANDLE)
    {
      EventUnregister(handle);
      handle = INVALID_HANDLE;
    }
  }

  /// <summary>
  /// Convert DebugLevel prefix to ETW level
  /// </summary>
  /// <param name="c"></param>
  /// <returns></returns>
  static inline char toLevel(char c)
  {
    switch (c)
    {
      case 'D':
      case 'T':
        c = 0x5;  // Verbose
        break;
      case 'I':
        c = 0x4;  // Informational
        break;
      case 'W':
        c = 0x3;  // Warning
        break;
      case 'E':
        c = 0x2;  // Error
        break;
      case 'F':
        c = 0x1;  // Critical
        break;
      default:
        c = 0;  // LogAlways
                // We log all events of unknown level
    }
    return c;
  }

  /// <summary>
  /// Pass string down to EventWriteString.
  /// </summary>
  /// <param name="s"></param>
  /// <param name="n"></param>
  /// <returns></returns>
  virtual std::streamsize xsputn(const char *s, std::streamsize n) override
  {
    // Ideally the string is expected to be UTF-16. But downstream processing
    // tools don't care. ASCII and UTF-8 strings are x2 times more compact
    // than UTF-16. The caller ensures that that the end of each buffer is
    // always padded with at least two zeros (UTF-16 'NUL' byte - U+0000).
    // That way the kernel processor properly identifies the end of buffer.
#  if 0
        // Enable this code to pass down string as UTF-16 instead of ASCII
        auto ws = to_utf16_string(s);
        wchar_t *buff = ws.c_str();
#  else
    char *buff = (char *)s;
#  endif

    if (handle != INVALID_HANDLE)
    {
#  if 0
        // FIXME: this needs a holistic solution in the higher-level API
        // Use first byte as a level hint
        UCHAR level = toLevel(buff[0]);
        buff += 2;
#  else
      UCHAR level = 0;  // LogAlways
#  endif
      EventWriteString(handle, level, 0, (PCWSTR)buff);
    }
    return n;
  }
};

/// <summary>
/// Class to send EventPayload to ETW provider
/// </summary>
class ETWStringStream : public TraceStringStream<ETWStringStreamBuffer>
{

protected:
  /// <summary>
  /// Open ETW provider buffer
  /// </summary>
  /// <param name="filename">ETW provider GUID string</param>
  /// <param name="mode"></param>
  virtual void open(const char *filename, ios_base::openmode mode = ios_base::out)
  {
    buffer.open(filename);
  }

  /// <summary>
  /// Open ETW provider buffer
  /// </summary>
  /// <param name="filename">ETW provider GUID string</param>
  /// <param name="mode"></param>
  virtual void open(GUID guid, ios_base::openmode mode = ios_base::out) { buffer.open(guid); }

  /// <summary>
  /// Close (unregister) ETW provider buffer
  /// </summary>
  virtual void close() { buffer.close(); }

public:
  /// <summary>
  /// Transform ETW provider name to provider GUID as described here:
  /// https://blogs.msdn.microsoft.com/dcook/2015/09/08/etw-provider-names-and-guids/
  /// </summary>
  /// <param name="providerName"></param>
  /// <returns></returns>
  static GUID GetProviderGuid(const char *providerName)
  {
    std::string name(providerName);
    std::transform(name.begin(), name.end(), name.begin(), ::toupper);

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

    unsigned long a  = (((((buffer2[3] << 8) + buffer2[2]) << 8) + buffer2[1]) << 8) + buffer2[0];
    unsigned short b = (unsigned short)((buffer2[5] << 8) + buffer2[4]);
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

  /// <summary>
  /// Specifies the string representation of ETW provider GUID to log to
  /// </summary>
  /// <param name="filename"></param>
  ETWStringStream(const char *providerName) : TraceStringStream<ETWStringStreamBuffer>()
  {
    if (providerName[0] == '{')
    {
      // It's a GUID passed down as a string. We literally convert it
      // from string to GUID and open ETW provider by GUID.
      open(providerName);
      return;
    }

    // Assume that we're dealing with a provider name:
    // - we generate a GUID hash using SHA-1
    // - we open ETW provider using that generated hash
    // Converting Provider names to ETW GUIDs is described here:
    /// https://blogs.msdn.microsoft.com/dcook/2015/09/08/etw-provider-names-and-guids/
    GUID guid = GetProviderGuid(providerName);
    open(guid);
  }

  /// <summary>
  /// Specifies the string representation of ETW provider GUID to log to
  /// </summary>
  /// <param name="filename"></param>
  ETWStringStream(GUID guid) : TraceStringStream<ETWStringStreamBuffer>() { open(guid); }

  virtual ~ETWStringStream() { close(); }
};

static std::ostream *new_ETWStringStream(const char *name)
{
  return new ETWStringStream(name);
}

}  // namespace stream

OPENTELEMETRY_END_NAMESPACE
#endif
