// Please DONOT touch this file.
// Any changes done here would be overwritten by pre-commit git hook

#include "opentelemetry/sdk/version/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace version
{
    const int MAJOR_VERSION = 2;
    const int MINOR_VERSION = 1;
    const int PATCH_VERSION = 1;
    const char* PRE_RELEASE = "test";
    const char* BUILD_METADATA = "NONE";
    const int COUNT_NEW_COMMITS = 6;
    const char* BRANCH = "version-hook-2";
    const char* COMMIT_HASH = "34bd479a77447780a2185bee6f7b8ffc377bb58c";
    const char* SHORT_VERSION = "2.1.1";
    const char* FULL_VERSION = "2.1.1-test-NONE-6-version-hook-2-34bd479a77447780a2185bee6f7b8ffc377bb58c";
    const char* BUILD_DATE = "Tue Nov 24 06:04:37 UTC 2020";
}
}
OPENTELEMETRY_END_NAMESPACE
