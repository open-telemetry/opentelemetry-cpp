// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

/*
  Keep includes in the same order as:
  - cd api/include
  - find . -name "*.h"
*/

// clang-format off
#include "opentelemetry/baggage/propagation/baggage_propagator.h"
#include "opentelemetry/baggage/baggage.h"
#include "opentelemetry/baggage/baggage_context.h"
#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/common/key_value_iterable.h"
#include "opentelemetry/common/key_value_iterable_view.h"
#include "opentelemetry/common/kv_properties.h"
#include "opentelemetry/common/macros.h"
#include "opentelemetry/common/spin_lock_mutex.h"
#include "opentelemetry/common/string_util.h"
#include "opentelemetry/common/timestamp.h"
#include "opentelemetry/config.h"
#include "opentelemetry/context/context_value.h"
#include "opentelemetry/context/propagation/composite_propagator.h"
#include "opentelemetry/context/propagation/global_propagator.h"
#include "opentelemetry/context/propagation/noop_propagator.h"
#include "opentelemetry/context/propagation/text_map_propagator.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/nostd/function_ref.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/nostd/type_traits.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/version.h"
// clang-format on

class AbiTextMapCarrier : public opentelemetry::context::propagation::TextMapCarrier
{
public:
  virtual opentelemetry::nostd::string_view Get(opentelemetry::nostd::string_view) const noexcept
  {
    return "";
  }

  virtual void Set(opentelemetry::nostd::string_view, opentelemetry::nostd::string_view) noexcept {}

  virtual bool Keys(
      opentelemetry::nostd::function_ref<bool(opentelemetry::nostd::string_view)>) const noexcept
  {
    return true;
  }
  virtual ~AbiTextMapCarrier() = default;
};

void do_abi_check_baggage()
{
  auto baggage = opentelemetry::baggage::Baggage::GetDefault();

  opentelemetry::context::Context ctx;

  auto propagator = opentelemetry::baggage::propagation::BaggagePropagator();
  AbiTextMapCarrier carrier;

  propagator.Extract(carrier, ctx);
  propagator.Inject(carrier, ctx);
}
