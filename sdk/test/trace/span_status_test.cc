// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <gtest/gtest.h>

#include <string>

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/sdk/trace/span_status.h"
#include "opentelemetry/trace/span_metadata.h"

using opentelemetry::nostd::string_view;
using opentelemetry::sdk::trace::ApplyStatusTransition;
using opentelemetry::trace::StatusCode;

namespace
{
constexpr const char *kDescription = "description";
constexpr bool kAccepted           = true;
constexpr bool kRejected           = false;

struct TransitionCase
{
  const char *name;
  StatusCode current;
  StatusCode code;
  bool accepted;
  StatusCode expected_code;
  const char *expected_description;
};

constexpr TransitionCase kCases[] = {
    {"UnsetToUnset", StatusCode::kUnset, StatusCode::kUnset, kRejected, StatusCode::kUnset, ""},
    {"UnsetToOk", StatusCode::kUnset, StatusCode::kOk, kAccepted, StatusCode::kOk, ""},
    {"UnsetToError", StatusCode::kUnset, StatusCode::kError, kAccepted, StatusCode::kError,
     kDescription},
    {"OkToUnset", StatusCode::kOk, StatusCode::kUnset, kRejected, StatusCode::kOk, ""},
    {"OkToOk", StatusCode::kOk, StatusCode::kOk, kRejected, StatusCode::kOk, ""},
    {"OkToError", StatusCode::kOk, StatusCode::kError, kRejected, StatusCode::kOk, ""},
    {"ErrorToUnset", StatusCode::kError, StatusCode::kUnset, kRejected, StatusCode::kError, ""},
    {"ErrorToOk", StatusCode::kError, StatusCode::kOk, kAccepted, StatusCode::kOk, ""},
    {"ErrorToError", StatusCode::kError, StatusCode::kError, kAccepted, StatusCode::kError,
     kDescription},
};

class SpanStatus : public ::testing::TestWithParam<TransitionCase>
{};

TEST_P(SpanStatus, FollowsSpecTransitionTable)
{
  const auto &test_case = GetParam();
  const auto transition = ApplyStatusTransition(test_case.current, test_case.code, kDescription);

  EXPECT_EQ(transition.accepted, test_case.accepted);
  EXPECT_EQ(transition.code, test_case.expected_code);
  EXPECT_EQ(transition.description, test_case.expected_description);
}

TEST_P(SpanStatus, DescriptionIsNeverNull)
{
  const auto &test_case = GetParam();
  const auto transition = ApplyStatusTransition(test_case.current, test_case.code, string_view{});

  EXPECT_NE(transition.description.data(), nullptr);
}

INSTANTIATE_TEST_SUITE_P(AllTransitions,
                         SpanStatus,
                         ::testing::ValuesIn(kCases),
                         [](const ::testing::TestParamInfo<TransitionCase> &info) {
                           return std::string{info.param.name};
                         });
}  // namespace
