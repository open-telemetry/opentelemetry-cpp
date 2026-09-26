#include "opentelemetry/baggage/baggage.h"
#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/context/propagation/global_propagator.h"

#include "opentelemetry/logs/provider.h"
#include "opentelemetry/trace/provider.h"
#include "opentelemetry/metrics/provider.h"

#include <Windows.h>

// privacy intruders
OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{
namespace detail
{
struct AbiBridgePrivacyIntruder
{
  using ThreadLocalContextStorageStack = ThreadLocalContextStorage::Stack;
  
  static ThreadLocalContextStorageStack &GetThreadLocalContextStorageStack()
  {
    static thread_local ThreadLocalContextStorage::Stack stack_{};
    return stack_;
  }
};
} // namespace detail
} // namespace context

namespace trace
{
namespace detail
{
struct AbiBridgePrivacyIntruder
{
  static nostd::shared_ptr<TraceState> GetDefaultTraceState()
  {
    static nostd::shared_ptr<TraceState> ts{new TraceState()};
    return ts;
  }
};
} // namespace detail
} // namespace trace
OPENTELEMETRY_END_NAMESPACE

// atom lifetime
namespace
{
struct AtomLifetime
{
  AtomLifetime() : atom{::AddAtomW(L"otelcpp-abi-bridge")} {}
  ~AtomLifetime()
  {
    if (atom != 0)
      ::DeleteAtom(atom);
  }
  
  ::ATOM atom{0};
};

const AtomLifetime bridgeAtom{};
}

extern "C" {

__declspec(dllexport) void OpenTelemetryAbiBridgeHook() {}

// - baggage
__declspec(dllexport) OPENTELEMETRY_NAMESPACE::nostd::shared_ptr<opentelemetry::baggage::Baggage> OpenTelemetryBaggageBaggageGetDefault()
{
  static OPENTELEMETRY_NAMESPACE::nostd::shared_ptr<opentelemetry::baggage::Baggage> baggage{new OPENTELEMETRY_NAMESPACE::baggage::Baggage()};
  return baggage;
}

// - runtime_context
__declspec(dllexport) OPENTELEMETRY_NAMESPACE::nostd::shared_ptr<opentelemetry::context::RuntimeContextStorage> &OpenTelemetryContextRuntimeContextGetStorage()
{
  static OPENTELEMETRY_NAMESPACE::nostd::shared_ptr<opentelemetry::context::RuntimeContextStorage> context(OPENTELEMETRY_NAMESPACE::context::GetDefaultStorage());
  return context;
}
__declspec(dllexport) OPENTELEMETRY_NAMESPACE::context::detail::AbiBridgePrivacyIntruder::ThreadLocalContextStorageStack &OpenTelemetryContextThreadLocalContextStorageStackGetStack()
{
  return OPENTELEMETRY_NAMESPACE::context::detail::AbiBridgePrivacyIntruder::GetThreadLocalContextStorageStack();
}

// - global_propagator
__declspec(dllexport) OPENTELEMETRY_NAMESPACE::nostd::shared_ptr<opentelemetry::context::propagation::TextMapPropagator> &OpenTelemetryContextPropagationGlobalTextMapPropagatorGetPropagator()
{
  static OPENTELEMETRY_NAMESPACE::nostd::shared_ptr<opentelemetry::context::propagation::TextMapPropagator> propagator(new OPENTELEMETRY_NAMESPACE::context::propagation::NoOpPropagator());
  return propagator;
}
__declspec(dllexport) OPENTELEMETRY_NAMESPACE::common::SpinLockMutex &OpenTelemetryContextPropagationGlobalTextMapPropagatorGetLock()
{
  static OPENTELEMETRY_NAMESPACE::common::SpinLockMutex lock;
  return lock;
}

// - logs provider
__declspec(dllexport) OPENTELEMETRY_NAMESPACE::nostd::shared_ptr<OPENTELEMETRY_NAMESPACE::logs::LoggerProvider> &OpenTelemetryLogsProviderGetProvider()
{
  static OPENTELEMETRY_NAMESPACE::nostd::shared_ptr<OPENTELEMETRY_NAMESPACE::logs::LoggerProvider> provider(new OPENTELEMETRY_NAMESPACE::logs::NoopLoggerProvider);
  return provider;
}

#if OPENTELEMETRY_ABI_VERSION_NO < 2
__declspec(dllexport) OPENTELEMETRY_NAMESPACE::nostd::shared_ptr<OPENTELEMETRY_NAMESPACE::logs::EventLoggerProvider> &OpenTelemetryLogsProviderGetEventProvider()
{
  static OPENTELEMETRY_NAMESPACE::nostd::shared_ptr<OPENTELEMETRY_NAMESPACE::logs::EventLoggerProvider> provider(new OPENTELEMETRY_NAMESPACE::logs::NoopEventLoggerProvider);
  return provider;
}
#endif

__declspec(dllexport) OPENTELEMETRY_NAMESPACE::common::SpinLockMutex &OpenTelemetryLogsProviderGetLock()
{
  static OPENTELEMETRY_NAMESPACE::common::SpinLockMutex lock;
  return lock;
}

// - metrics provider
__declspec(dllexport) OPENTELEMETRY_NAMESPACE::nostd::shared_ptr<OPENTELEMETRY_NAMESPACE::metrics::MeterProvider> &OpenTelemetryMetricsProviderGetProvider()
{
  static OPENTELEMETRY_NAMESPACE::nostd::shared_ptr<OPENTELEMETRY_NAMESPACE::metrics::MeterProvider> provider(new OPENTELEMETRY_NAMESPACE::metrics::NoopMeterProvider);
  return provider;
}
__declspec(dllexport) OPENTELEMETRY_NAMESPACE::common::SpinLockMutex &OpenTelemetryMetricsProviderGetLock()
{
  static OPENTELEMETRY_NAMESPACE::common::SpinLockMutex lock;
  return lock;
}

// - trace provider
__declspec(dllexport) OPENTELEMETRY_NAMESPACE::nostd::shared_ptr<OPENTELEMETRY_NAMESPACE::trace::TracerProvider> &OpenTelemetryTraceProviderGetProvider()
{
  static OPENTELEMETRY_NAMESPACE::nostd::shared_ptr<OPENTELEMETRY_NAMESPACE::trace::TracerProvider> provider(new OPENTELEMETRY_NAMESPACE::trace::NoopTracerProvider);
  return provider;
}

__declspec(dllexport) OPENTELEMETRY_NAMESPACE::common::SpinLockMutex &OpenTelemetryTraceProviderGetLock()
{
  static OPENTELEMETRY_NAMESPACE::common::SpinLockMutex lock;
  return lock;
}

// - trace_state
__declspec(dllexport) OPENTELEMETRY_NAMESPACE::nostd::shared_ptr<OPENTELEMETRY_NAMESPACE::trace::TraceState> OpenTelemetryTraceTraceStateGetDefault()
{
  return OPENTELEMETRY_NAMESPACE::trace::detail::AbiBridgePrivacyIntruder::GetDefaultTraceState();
}

}
