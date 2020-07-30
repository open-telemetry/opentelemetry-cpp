// Copyright 2020, OpenTelemetry Authors
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

#pragma once

#include "opentelemetry/context/context.h"
#include "opentelemetry/trace/propagation/httptextformat.h"
#include "opentelemetry/nostd/span.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace propagators
{
    // CompositeHTTPPropagator provides a mechanism for combining multiple
    // propagators into a single one.
    template <typename T>
    class CompositeHTTPPropagator : public trace::propagation::HTTPTextFormat<T>
        public:
            // Rules that manages how context will be extracted from carrier.
            using Getter = nostd::string_view(*)(const T &carrier, nostd::string_view trace_type);

            // Rules that manages how context will be injected to carrier.
            using Setter = void(*)(T &carrier, nostd::string_view trace_type,nostd::string_view trace_description);

            // Initializes a Composite Http Propagator with given propagators
            CompositeHTTPPropagator(nostd::span<trace::propagation::HTTPTextFormat> &propagators) {
                this.propagators_ = propagators;
            }

            // Run each of the configured propagators with the given context and carrier.
            // Propagators are run in the order they are configured, if multiple
            // propagators write the same context key, the propagator later in the list
            // will override previous propagators.
            // See opentelemetry.trace.propagation.httptextformat.HTTPTextFormat.extract
            Extract(Getter get_from_carrier, const T &carrier, Context &context) {
                for (nostd::span<trace::propagation::HTTPTextFormat>::iterator it = propagators_.begin(); it != propagators_.end(); it++) {
                    context = it->Extract(get_from_carrier, carrier, context);
                }
                return context;
            }

            // Run each of the configured propagators with the given context and carrier.
            // Propagators are run in the order they are configured, if multiple
            // propagators write the same carrier key, the propagator later in the list
            // will override previous propagators.
            // See `opentelemetry.trace.propagation.httptextformat.HTTPTextFormat.inject`
            Inject(Setter set_from_carrier, T &carrier, const Context &context) {
                for (nostd::span<trace::propagation::HTTPTextFormat>::iterator it = propagators_.begin(); it != propagators_.end(); it++) {
                    it->Inject(get_from_carrier, carrier, context);
                }
            }
        private:
            nostd::span<trace::propagation::HTTPTextFormat> propagators_;
    }
}
