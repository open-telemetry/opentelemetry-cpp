#pragma once
#include <chrono>
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
 *
//  */
// static const struct LogRecord defaultRecord = {
//   timestamp : std::chrono::steady_clock::now().time_since_epoch(),
//   trace_id : 0,
//   span_id : 0,
//   severity_number : 0
// };

class Logger
{
private:
  std::vector<LogRecord> records;

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
   * @param some_obj the log object of user templated type that is logged.
   * @throws No exceptions under any circumstances.
   */

  /** A method for an unstructured log **/
  virtual void log(nostd::string_view name, Severity sev, nostd::string_view msg) noexcept = 0;

  /** Methods for structured logs **/
  // (name, sev, KeyValueIterable) overloads
  virtual void log(nostd::string_view name                    = "",
                   Severity sev                               = Severity::NONE,
                   const common::KeyValueIterable &attributes = {}) noexcept = 0;

  /*
  void log(Severity sev, const common::KeyValueIterable &attributes) noexcept
  {
    log("", sev, attributes);
  }
  void log(nostd::string_view name, const common::KeyValueIterable &attributes) noexcept
  {
    log(name, Severity::NONE, attributes);
  }
  void log(const common::KeyValueIterable &attributes) noexcept { log("", attributes); }
  */

  // (name, sev, KeyValueIterable) overloads
  template <class T,
            nostd::enable_if_t<common::detail::is_key_value_iterable<T>::value> * = nullptr>
  void log(nostd::string_view name = "",
           Severity sev            = Severity::NONE,
           const T &attributes     = {}) noexcept
  {
    log(name, sev, common::KeyValueIterableView<T>(attributes));
  }

  void log(nostd::string_view name,
           std::initializer_list<std::pair<nostd::string_view, common::AttributeValue>>
               attributes) noexcept
  {
    // log(name, nostd::span<const std::pair<nostd::string_view,
    // common::AttributeValue>>{attributes.begin(), attributes.end()});
  }

  /* A logging statement that takes in a LogRecord.
   * A default LogRecord that will be assigned if no parameters are passed to Logger's .log() method
   * which should at minimum contain the trace_id, span_id, and current timestamp
   * TODO: correlate timestamp, traceid and spanid at the minimum to Log Record
   *
   */
  virtual void log(const LogRecord &record = {}) noexcept = 0;

  /*** Future additions*/
  /* templated method for objects / custom types (e.g. JSON, XML, custom classes, etc) (for later)
   */
  // template<class T> void log(T &some_obj) noexcept override;
};
}  // namespace logs
OPENTELEMETRY_END_NAMESPACE
