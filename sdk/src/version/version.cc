// Please DONOT touch this file.
// Any changes done here would be overwritten by pre-commit git hook

#include "opentelemetry/sdk/version/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace version
{
const int MAJOR_VERSION     = 1;
const int MINOR_VERSION     = 4;
const int PATCH_VERSION     = 1;
const char *PRE_RELEASE     = "NONE";
const char *BUILD_METADATA  = "NONE";
const int COUNT_NEW_COMMITS = 20;
const char *BRANCH          = "pre_release_1.4.1";
const char *COMMIT_HASH     = "7f4eaac3c296d5198e5b4be3a2222ade85551759";
const char *SHORT_VERSION   = "1.4.1";
const char *FULL_VERSION =
    "1.4.1-NONE-NONE-20-pre_release_1.4.1-7f4eaac3c296d5198e5b4be3a2222ade85551759";
const char *BUILD_DATE = "Sun Jun 19 16:34:17 UTC 2022";
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
