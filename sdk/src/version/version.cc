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
const int PATCH_VERSION     = 2;
const char *PRE_RELEASE     = "NONE";
const char *BUILD_METADATA  = "NONE";
const int COUNT_NEW_COMMITS = 37;
const char *BRANCH          = "pre_release_1.8.2";
const char *COMMIT_HASH     = "435ce60f233b6718aaa04bb7068dd641b536299b";
const char *SHORT_VERSION   = "1.8.2";
const char *FULL_VERSION =
    "1.8.2-NONE-NONE-37-pre_release_1.8.2-435ce60f233b6718aaa04bb7068dd641b536299b";
const char *BUILD_DATE = "Tue 31 Jan 2023 04:01:10 PM UTC";
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
