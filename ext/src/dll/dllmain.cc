// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include <windows.h>

// Include API header files here for exporting
#include <opentelemetry/trace/provider.h>

#if defined(ENABLE_LOGS_PREVIEW)
#  include <opentelemetry/logs/logger_provider.h>
#endif  // ENABLE_LOGS_PREVIEW

extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
  UNREFERENCED_PARAMETER(hInstance);
  UNREFERENCED_PARAMETER(dwReason);
  UNREFERENCED_PARAMETER(lpReserved);

  return TRUE;
}
