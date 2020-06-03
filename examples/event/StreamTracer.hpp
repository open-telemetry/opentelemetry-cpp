#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <mutex>

#include <opentelemetry/trace/key_value_iterable_view.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/span_id.h>
#include <opentelemetry/trace/trace_id.h>
#include <opentelemetry/trace/tracer_provider.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <memory>
#include <cstdint>

#include "utils.hpp"

#include "EventProtocols.hpp"

namespace core     = opentelemetry::core;
namespace trace    = opentelemetry::trace;
namespace protocol = OPENTELEMETRY_NAMESPACE::protocol;

OPENTELEMETRY_BEGIN_NAMESPACE

/// <summary>
/// stream namespace provides no-exporter header-only implementation of local stream tracers:
/// - file
/// - ETW
/// - OutputDebugString
/// - console
///
/// </summary>
namespace stream
{

/// <summary>
/// TraceStreamType allows to redirect the string stream to various pipe destinations.
/// Events emitted via Tracer to stream may undergo intermediate stream-specific transform.
/// </summary>
enum class TraceStreamType
{
  ST_NULL,               // Cross-platform /dev/null or NUL
  ST_File_Log,           // Cross-platform file - plain text k=v
  ST_File_JSON,          // Cross-platform file - JSON

  // Windows only:
  ST_OutputDebugString,  // OutputDebugString
  ST_ETW,                // ETW API (XML)
  ST_ETW_JSON,           // ETW API (JSON)

  // Unix only:
  ST_SYSLOG,             // syslog            - *nix only

  ST_CONSOLE,            // console (std::cout)
  ST_USER,               // Custom
  ST_MAX
};

/// <summary>
/// Base class that enforces the following contract:
/// http://www.cplusplus.com/reference/streambuf/streambuf/xsputn/
/// </summary>
class TraceStreamBuffer : public std::streambuf
{
public:
  TraceStreamBuffer() : std::streambuf(){};
  virtual std::streamsize xsputn(const char *s, std::streamsize n) = 0;
};

/// <summary>
/// Convenience template to simplify implementation of custom string streams
/// </summary>
template <class T>
class TraceStringStream : public std::ostream
{
protected:
  T buffer;

public:

  /// <summary>
  /// Constructor that initializes custom stream buffer implementation
  /// </summary>
  /// <returns>TraceStringStream<T></returns>
  TraceStringStream<T>() : std::ostream(&buffer){};

  /// <summary>
  /// Support for more efficient binary transfer of data
  /// </summary>
  /// <param name="data">Tuple containing Event Data</param>
  /// <returns></returns>
  virtual TraceStringStream<T> &operator<<(protocol::EventData data)
  {
      return *this;
  }
};

/// <summary>
/// Implementation of OutputDebugString stream
/// </summary>
class OutputDebugStringStreamBuffer : public TraceStreamBuffer
{
public:

  /// <summary>
  /// Pass UTF-8 string down to OutputDebugStringA
  /// </summary>
  /// <param name="s">C-string to send to stream</param>
  /// <param name="n">String size</param>
  /// <returns></returns>
  virtual std::streamsize xsputn(const char *s, std::streamsize n) override
  {
#ifdef _WIN32
    OutputDebugStringA(s);
    return n;
#else
    /* Not implemented for platforms other than Windows */
    return 0;
#endif
  }

};

/// <summary>
/// Output stream wrapper for OutputDebugString
/// </summary>
class OutputDebugStringStream : public TraceStringStream<OutputDebugStringStreamBuffer>
{};

/// <summary>
/// High-performance optimized implementation of NULL output stream buffer
/// </summary>
class NullStreamBuffer : public std::streambuf
{
public:
  NullStreamBuffer() : std::streambuf(){};
  virtual std::streamsize xsputn(const char *s, std::streamsize n) { return 0; };
};

/// <summary>
/// High performance NULL output stream
/// </summary>
class NullStringStream : public TraceStringStream<NullStreamBuffer>
{
public:
  /// <summary>
  /// Fastest method to ignore anything sent to stream is to set the badbit
  /// </summary>
  /// <returns>stream constructed with badbit set</returns>
  NullStringStream() : TraceStringStream<NullStreamBuffer>() { setstate(std::ios_base::badbit); }
};

#ifdef _WIN32
/// <summary>
/// Forward declaration of ETWStringStream allocator
/// </summary>
/// <param name="name">ETW Provider Name or GUID</param>
/// <returns>ETW String Stream instance</returns>
static std::ostream *new_ETWStringStream(const char *name);
#endif

class Span;

/// <summary>
/// stream::Tracer class that allows to send spans to stream
/// </summary>
class Tracer : public trace::Tracer
{
  /// <summary>
  /// Parent provider of this Tracer
  /// </summary>
  trace::TracerProvider &provider;

  /// <summary>
  /// Stream type
  /// </summary>
  TraceStreamType stype;

  /// <summary>
  /// Stream filename (optional, used for file streams)
  /// </summary>
  std::string filename;

  /// <summary>
  /// Custom stream instance
  /// </summary>
  std::unique_ptr<std::ostream> stream;

  /// <summary>
  /// Reference to current ostream object
  /// </summary>
  std::ostream &sout;

  /// <summary>
  /// Converter that transforms EventData to string suitable for given stream type
  /// </summary>
  protocol::EventConverter &converter;

  /// <summary>
  /// Codec converter factory for a given stream type
  /// </summary>
  protocol::EventConverter &init_converter()
  {
    switch (stype)
    {

      case TraceStreamType::ST_NULL: {
        static protocol::NullConverter nullConverter;
        return nullConverter;
      }

      case TraceStreamType::ST_File_Log: {
        static protocol::PlainKVConverter kvConverter;
        return kvConverter;
      }

      case TraceStreamType::ST_File_JSON: {
        static protocol::JsonConverter jsonConverter;
        return jsonConverter;
      }

      case TraceStreamType::ST_OutputDebugString: {
        static protocol::PlainKVConverter kvConverter;
        return kvConverter;
      }

#ifdef _WIN32
      case TraceStreamType::ST_ETW: {
        static protocol::ETWEventConverter etwConverter;
        return etwConverter;
      }
#endif

      case TraceStreamType::ST_SYSLOG: {
        // TODO: not implemented
        static protocol::PlainKVConverter kvConverter;
        return kvConverter;
      }

      case TraceStreamType::ST_CONSOLE: {
        static protocol::PlainKVConverter kvConverter;
        return kvConverter;
      }

      default:
        // nobrk

      case TraceStreamType::ST_USER: {
        // TODO: not implemented
        static protocol::NullConverter nullConverter;
        return nullConverter;
      }
    }
  }

  /// <summary>
  /// ostream initializer based on current stype
  /// </summary>
  std::ostream &init_ostream()
  {
    switch (stype)
    {

      case TraceStreamType::ST_NULL: {
        static NullStringStream stream;
        return stream;
      };

      case TraceStreamType::ST_File_Log:
      case TraceStreamType::ST_File_JSON:
      {
        // TODO: consider LogFileStream with automatic logs rotation
        auto s = new std::ofstream();
        s->open(filename, std::ios_base::out);
        stream.reset(s);
        return *stream.get();
      };

      case TraceStreamType::ST_OutputDebugString: {
        static OutputDebugStringStream ods;
        return ods;
      };

#ifdef _WIN32
      case TraceStreamType::ST_ETW: {
        auto ptr = new_ETWStringStream(filename.c_str());
        //
        // TODO: when new ETW trace stream is created - use EventActivityIdControl
        // to propagate TraceId 128-bit value as Activity ID guid.
        //
        // Need to decide what has to be done for each individual SpanId :
        // do we propagate this as an attribute on span or extract this into
        // 'special' attributes of ETW event?
        //
        stream.reset(ptr);
        return *stream;
      };
#endif

      case TraceStreamType::ST_SYSLOG: {
        /* TODO: not implemented */
        static NullStringStream stream;
        return stream;
      };

      case TraceStreamType::ST_CONSOLE: {
        return std::cout;
      };

      case TraceStreamType::ST_USER: {
        /* TODO: not implemented */
        static NullStringStream stream;
        return stream;
      };

      default:
        break;
    };
    return std::cout;
  }

public:

  /// <summary>
  /// Tracer constructor
  /// </summary>
  /// <param name="parent">Parent TraceProvider</param>
  /// <param name="streamType">Stream type</param>
  /// <param name="arg2">Optional 2nd argument, e.g. filename</param>
  /// <returns>Tracer instance</returns>
  Tracer(trace::TracerProvider &parent,
         TraceStreamType streamType = TraceStreamType::ST_CONSOLE,
         std::string_view arg2      = "")
      : trace::Tracer(),
        provider(parent),
        stype(streamType),
        filename(arg2.data(), arg2.size()),
        converter(init_converter()),
        sout(init_ostream())
  {}

  /**
   * Q:
   *
   * Is there a reason why we require unique_ptr here? In most cases the lifetime of a span should be
   * managed by Tracer: every span has a parent tracer. Client code can hold a non-owning reference
   * to Span. There could be multiple clients obtaining a reference to the same Span object as well.
   * All clients must not use a Span if the Tracer is destroyed...
   */

  /// <summary>
  /// 
  /// </summary>
  /// <param name="name">Span name</param>
  /// <param name="options">Span options</param>
  /// <returns>Span</returns>
  virtual nostd::unique_ptr<trace::Span> StartSpan(
      nostd::string_view name,
      const trace::StartSpanOptions &options = {}) noexcept override
  {
    return trace::to_span_ptr<Span, Tracer>(this, name, options);
  }

  /// <summary>
  /// Force flush data to Tracer, spending up to given amount of microseconds to flush.
  /// </summary>
  /// <param name="timeout">Allow Tracer to drop data if timeout is reached</param>
  /// <returns>void</returns>
  virtual void ForceFlushWithMicroseconds(uint64_t timeout) noexcept override
  {

  }

  /// <summary>
  /// Close tracer, spending up to given amount of microseconds to flush and close.
  /// </summary>
  /// <param name="timeout">Allow Tracer to drop data if timeout is reached</param>
  /// <returns></returns>
  virtual void CloseWithMicroseconds(uint64_t timeout) noexcept override
  {
      sout.flush();
      if (stream)
      {
          // TODO: do we need to perform extra actions to close it
          // or should we assume that destructor takes care of it?
      }
  }

  /// <summary>
  /// Add event data to span associated with tracer
  /// </summary>
  /// <param name="span"></param>
  /// <param name="name"></param>
  /// <param name="timestamp"></param>
  /// <param name="attributes"></param>
  /// <returns></returns>
  void AddEvent(Span &span, nostd::string_view name,
                core::SystemTimestamp timestamp,
                const trace::KeyValueIterable &attributes) noexcept
  {
    (void)span;
    sout << converter.convert({name, timestamp, attributes});
  }

  /// <summary>
  /// Add event data to span associated with tracer
  /// </summary>
  /// <param name="span"></param>
  /// <param name="name"></param>
  /// <param name="timestamp"></param>
  /// <returns></returns>
  void AddEvent(Span &span, nostd::string_view name, core::SystemTimestamp timestamp) noexcept
  {
    (void)span;
    sout << converter.convert({name, timestamp, trace::NullKeyValueIterable()});
  }

  /// <summary>
  /// Add event data to span associated with tracer
  /// </summary>
  /// <param name="span"></param>
  /// <param name="name"></param>
  void AddEvent(Span &span, nostd::string_view name)
  {
    (void)span;
    sout << converter.convert(
        {name, std::chrono::system_clock::now(), trace::NullKeyValueIterable()});
  }

};

/// <summary>
/// stream::Span allows to send event data to stream
/// </summary>
class Span : public trace::Span
{

protected:

  /// <summary>
  /// Parent (Owner) Tracer of this Span
  /// </summary>
  Tracer &owner;

public:

  /// <summary>
  /// Span constructor
  /// </summary>
  /// <param name="owner">Owner Tracer</param>
  /// <param name="name">Span name</param>
  /// <param name="options">Span options</param>
  /// <returns>Span</returns>
  Span(Tracer &owner, nostd::string_view name, const trace::StartSpanOptions &options) noexcept
      : trace::Span(), owner(owner)
  {
    (void)options;
  }

  ~Span()
  {
      End();
  }

  /// <summary>
  /// Add named event with no attributes
  /// </summary>
  /// <param name="name"></param>
  /// <returns></returns>
  void AddEvent(nostd::string_view name) noexcept
  {
      owner.AddEvent(*this, name);
  }

  /// <summary>
  /// Add named event with custom timestamp
  /// </summary>
  /// <param name="name"></param>
  /// <param name="timestamp"></param>
  /// <returns></returns>
  void AddEvent(nostd::string_view name, core::SystemTimestamp timestamp) noexcept
  {
    owner.AddEvent(*this, name, timestamp);
  }

  /// <summary>
  /// Add named event with custom timestamp and attributes
  /// </summary>
  /// <param name="name"></param>
  /// <param name="timestamp"></param>
  /// <param name="attributes"></param>
  /// <returns></returns>
  void AddEvent(nostd::string_view name,
                core::SystemTimestamp timestamp,
                const trace::KeyValueIterable &attributes) noexcept
  {
    owner.AddEvent(*this, name, timestamp, attributes);
  }

  /// <summary>
  /// Set Span status
  /// </summary>
  /// <param name="code"></param>
  /// <param name="description"></param>
  /// <returns></returns>
  void SetStatus(trace::CanonicalCode code, nostd::string_view description) noexcept
  {
      // TODO: not implemented
  }

  /// <summary>
  /// Update Span name
  /// </summary>
  /// <param name="name"></param>
  /// <returns></returns>
  void UpdateName(nostd::string_view name) noexcept
  {
      // TODO: not implemented
  }

  /// <summary>
  /// End Span
  /// </summary>
  /// <returns></returns>
  void End() noexcept
  {
      // TODO: signal this to owner
  }

  /// <summary>
  /// Check if Span is recording data
  /// </summary>
  /// <returns></returns>
  bool IsRecording() const noexcept
  {
      // TODO: not implemented
      return true;
  }

  /// <summary>
  /// Get Owner tracer of this Span
  /// </summary>
  /// <returns></returns>
  trace::Tracer &tracer() const noexcept
  {
      return this->owner;
  };

};

/// <summary>
/// stream::TraceProvider
/// </summary>
class TracerProvider : public trace::TracerProvider
{
public:

  /// <summary>
  /// Obtain a Tracer of given type (name) and supply extra argument arg2 to it.
  /// </summary>
  /// <param name="name">Tracer Type</param>
  /// <param name="args">Tracer arguments</param>
  /// <returns></returns>
  virtual nostd::shared_ptr<trace::Tracer> GetTracer(nostd::string_view name,
                                                     nostd::string_view args = "")
  {
    TraceStreamType stype = TraceStreamType::ST_NULL;
    auto h                = utils::hashCode(name.data());
    // Map from string name to TraceStreamType
    std::unordered_map<uint32_t, TraceStreamType> stypes =
    {
      {CONST_HASHCODE(console),   TraceStreamType::ST_CONSOLE},
      {CONST_HASHCODE(CON),       TraceStreamType::ST_CONSOLE},
      {CONST_HASHCODE(con),       TraceStreamType::ST_CONSOLE},
#ifdef _WIN32
      {CONST_HASHCODE(etw),       TraceStreamType::ST_ETW},
      {CONST_HASHCODE(ETW),       TraceStreamType::ST_ETW},
      {CONST_HASHCODE(debug),     TraceStreamType::ST_OutputDebugString},
      {CONST_HASHCODE(DEBUG),     TraceStreamType::ST_OutputDebugString},
#endif
      {CONST_HASHCODE(NUL),       TraceStreamType::ST_NULL},
      {CONST_HASHCODE(/dev/null), TraceStreamType::ST_NULL},
      {CONST_HASHCODE(file),      TraceStreamType::ST_File_Log},
      {CONST_HASHCODE(JSON),      TraceStreamType::ST_File_JSON},
      {CONST_HASHCODE(json),      TraceStreamType::ST_File_JSON},
    };
    // TODO: add more types in here and allow user-registered streams
    auto it = stypes.find(h);
    if (it != stypes.end())
      stype = it->second;
    return nostd::shared_ptr<trace::Tracer>{new (std::nothrow) Tracer(*this, stype, args)};
  }
};

}  // namespace stream
OPENTELEMETRY_END_NAMESPACE

// Windows-only implementation of ETW stream
#include "ETWStringStream.hpp"
