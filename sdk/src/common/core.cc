#include "opentelemetry/context/runtime_context.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/version.h"

#if defined(HAVE_ABSEIL)
namespace absl
{
namespace variant_internal
{
void __cdecl ThrowBadVariantAccess(){};
}
}  // namespace absl
#endif

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

thread_local ThreadLocalContext::Stack ThreadLocalContext::stack_ = ThreadLocalContext::Stack();

}  // namespace context
OPENTELEMETRY_END_NAMESPACE
