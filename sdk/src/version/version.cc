// Please DONOT touch this file.
// Any changes done here would be overwritten by pre-commit git hook

#include "opentelemetry/sdk/version/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace sdk
{
namespace version
{
const int MAJOR_VERSION     = 1;
const int MINOR_VERSION     = 7;
const int PATCH_VERSION     = 0;
const char *PRE_RELEASE     = "NONE";
const char *BUILD_METADATA  = "NONE";
const int COUNT_NEW_COMMITS = 35;
const char *BRANCH          = "pre_release_1.7.0";
const char *COMMIT_HASH     = "dd7e257b6de71eeaf9e3149530962301705b9a0d";
const char *SHORT_VERSION   = "1.7.0";
const char *FULL_VERSION =
    "1.7.0-NONE-NONE-35-pre_release_1.7.0-dd7e257b6de71eeaf9e3149530962301705b9a0d";
const char* BUILD_DATE = "Fri 28 Oct 2022 06:22:43 PM UTC";
}  // namespace version
}  // namespace sdk
OPENTELEMETRY_END_NAMESPACE
