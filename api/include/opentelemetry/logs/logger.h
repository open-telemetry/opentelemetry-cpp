#pragma once

#include <chrono>
#include <map>
#include <vector>

#include "opentelemetry/common/key_value_iterable.h"  // fix after moving to "common"
#include "opentelemetry/logs/log_record.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace logs
{
/**
 * Handles log record creation.
 **/
class Logger
{
public:
  virtual ~Logger() = default;

  /* Returns the name of the logger */
  virtual nostd::string_view getName() = 0;

  /**
   * Creates a log message with the passed characteristics.
   *
   * @param name the name of the log event.
   * @param sev the severity level of the log event.
   * @param msg the string message of the log (perhaps support std::fmt or fmt-lib format).
   * @param record the log record (object type LogRecord) that is logged.
   * @param attributes the attributes, stored as a 2D list of key/value pairs, that are associated
   * with this log.
   * @throws No exceptions under any circumstances.
   */

  // Potential bool isenabled(severity): Break into seperate future PRs
  // Log issue about macros (disabling log events below certain level)

  /* The below method is a logging statement that takes in a LogRecord.
   * A default LogRecord that will be assigned if no parameters are passed to Logger's .log() method
   * which should at minimum assign the trace_id, span_id, and timestamp
   */
  virtual void log(const LogRecord &record) noexcept = 0;

  // First, finalize core,
  // Afterwards, come back and add overloads

  /** Overloaded methods for unstructured logging **/
  void log(nostd::string_view msg) noexcept
  {
    log(Severity::kNone, msg);  // Set severity to NONE as default then call the log method below
  }

  void log(Severity sev, nostd::string_view msg) noexcept
  {
    // Set timestamp to now as default then call the log method below
    log(sev, msg, core::SystemTimestamp(std::chrono::system_clock::now()));
  }

  void log(Severity sev, nostd::string_view msg, core::SystemTimestamp time) noexcept
  {
    LogRecord r;
    r.body      = msg;
    r.severity  = sev;
    r.timestamp = time;

    log(r);  // converts to a LogRecord object, then calls log(LogRecord)
  }

  /** Overloaded methods for structured logging**/
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void log(nostd::string_view name = "",
           Severity sev            = Severity::kNone,
           const T &attributes     = {}) noexcept
  {
    log(name, sev, common::KeyValueIterableView<T>(attributes));
  }

  void log(nostd::string_view name,
           Severity sev,
           const common::KeyValueIterable &attributes) noexcept
  {
    LogRecord r;
    r.name       = name;
    r.severity   = sev;
    r.attributes = attributes;

    log(r);  // converts to a LogRecord object, then calls log(LogRecord)
  }

  /** Future enhancement: templated method for objects / custom types (e.g. JSON, XML, custom
   * classes, etc) **/
  // template<class T> void log(T &some_obj) noexcept override;
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
