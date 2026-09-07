// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <gmock/gmock.h>
#include <memory>
#include <string>

#include "opentelemetry/ext/http/client/http_client.h"
#include "opentelemetry/test_common/ext/http/client/nosend/http_client_nosend.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace exporter
{
namespace otlp
{

namespace
{

// Captures the body of the one request the session is asked to send, and
// completes it. The session must outlive the export under test.
inline void CaptureRequestBody(const std::shared_ptr<ext::http::client::nosend::Session> &session,
                               std::string &captured_body)
{
  auto *raw_session = session.get();
  EXPECT_CALL(*session, SendRequest)
      .WillOnce([&captured_body,
                 raw_session](const std::shared_ptr<ext::http::client::EventHandler> &callback) {
        const auto &body = raw_session->GetRequest()->body_;
        captured_body.assign(body.begin(), body.end());
        ext::http::client::nosend::Response response;
        response.Finish(*callback.get());
      });
}

}  // namespace

}  // namespace otlp
}  // namespace exporter
OPENTELEMETRY_END_NAMESPACE
