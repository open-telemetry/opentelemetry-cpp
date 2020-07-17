#ifdef _WIN32
#  pragma once

// TODO: adjust the code to use MultiByteToWaideChar() and WideCharToMultiByte()
// Currently _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING is defined
// to suppress the codecvt_utf16 warning.

#  include "StreamTracer.hpp"

#  include "opentelemetry/event/UUID.hpp"

#  include "utils.hpp"

#  include <codecvt>
#  include <locale>
#  include <string>
#  include <ostream>

OPENTELEMETRY_BEGIN_NAMESPACE

namespace stream
{

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
    OPENTELEMETRY_NAMESPACE::event::UUID uuid{path};
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
    GUID guid = utils::GetProviderGuid(providerName);
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
