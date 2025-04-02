// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/version.h"
#include "opentelemetry/sdk/version/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace version
{
const int GetMajorVersion()     { return major_version;  }
const int GetMinorVersion()     { return minor_version;  }
const int GetPatchVersion()     { return patch_version;  }
const char* GetPreRelease()     { return pre_release;    }
const char* GetBuildMetadata()  { return build_metadata; }
const char* GetShortVersion()  { return short_version;  }
const char* GetFullVersion()    { return full_version;   }
const char* GetBuildData()      { return build_date;     }
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
