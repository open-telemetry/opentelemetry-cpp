// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/ext/http/client/curl/http_client_curl.h"

bool opentelemetry::ext::http::client::curl::Session::CancelSession() noexcept
{
  curl_operation_->Abort();
  http_client_.CleanupSession(session_id_);
  return true;
}

bool opentelemetry::ext::http::client::curl::Session::FinishSession() noexcept
{
  curl_operation_->Finish();
  http_client_.CleanupSession(session_id_);
  return true;
}
