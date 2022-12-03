// Please DONOT touch this file.
// Any changes done here would be overwritten by pre-commit git hook

#include "opentelemetry/sdk/version/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace version
{
const int MAJOR_VERSION     = 1;
const int MINOR_VERSION     = 8;
const int PATCH_VERSION     = 0;
const char *PRE_RELEASE     = "NONE";
const char *BUILD_METADATA  = "NONE";
const int COUNT_NEW_COMMITS = 31;
const char *BRANCH          = "pre_release_1.8.0";
const char *COMMIT_HASH     = "06b4795ba2cf898efda4ee34d7b40fb6a2968b95";
const char *SHORT_VERSION   = "1.8.0";
const char *FULL_VERSION =
    "1.8.0-NONE-NONE-31-pre_release_1.8.0-06b4795ba2cf898efda4ee34d7b40fb6a2968b95";
const char *BUILD_DATE = "Sun 27 Nov 2022 02:32:48 PM UTC";
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
