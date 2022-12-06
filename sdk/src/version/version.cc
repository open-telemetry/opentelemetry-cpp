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
const int PATCH_VERSION     = 1;
const char *PRE_RELEASE     = "NONE";
const char *BUILD_METADATA  = "NONE";
const int COUNT_NEW_COMMITS = 11;
const char *BRANCH          = "pre_release_1.8.1";
const char *COMMIT_HASH     = "36ddf46ddea46a00d93aa647c821d7b6045ee42a";
const char *SHORT_VERSION   = "1.8.1";
const char *FULL_VERSION =
    "1.8.1-NONE-NONE-11-pre_release_1.8.1-36ddf46ddea46a00d93aa647c821d7b6045ee42a";
const char *BUILD_DATE = "Sun 04 Dec 2022 10:11:53 AM UTC";
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
