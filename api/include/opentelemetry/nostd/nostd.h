#pragma once

#include "opentelemetry/version.h"

#ifndef HAVE_CPP_STDLIB
// OpenTelemetry backport of STL C++20 types
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/span.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/unique_ptr.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/variant.h"
#else
// Standard Library implementation
#include "stltypes.h"
#endif
