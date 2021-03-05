#include <initializer_list>
#include <memory>
#include <vector>
#include "opentelemetry/trace/propagation/text_map_propagator.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace trace
{
namespace propagation
{

template <typename T>
class CompositePropagator : public TextMapPropagator<T>
{
public:
  CompositePropagator(std::vector<std::shared_ptr<TextMapPropagator<T>>> &propagators)
      : propagators_(propagators)
  {}
  // Rules that manages how context will be extracted from carrier.
  using Getter = nostd::string_view (*)(const T &carrier, nostd::string_view trace_type);

  // Rules that manages how context will be injected to carrier.
  using Setter = void (*)(T &carrier,
                          nostd::string_view trace_type,
                          nostd::string_view trace_description);

  /**
   * Run each of the configured propagators with the given context and carrier.
   * Propagators are run in the order they are configured, so if multiple
   * propagators write the same carrier key, the propagator later in the list
   * will "win".
   *
   * @param setter Rules that manages how context will be injected to carrier.
   * @param carrier Carrier into which context will be injected
   * @param context Context to inject
   *
   */

  void Inject(Setter setter, T &carrier, const context::Context &context) noexcept override
  {
    for (auto &p : propagators_)
    {
      p->Inject(setter, carrier, context);
    }
  }

  /**
   * Run each of the configured propagators with the given context and carrier.
   * Propagators are run in the order they are configured, so if multiple
   * propagators write the same context key, the propagator later in the list
   * will "win".
   *
   * @param setter Rules that manages how context will be extracte from carrier.
   * @param context Context to add values to
   * @param carrier Carrier from which to extract context
   */
  context::Context Extract(Getter getter,
                           const T &carrier,
                           context::Context &context) noexcept override
  {
    for (auto &p : propagators_)
    {
      context = p->Extract(getter, carrier, context);
    }
    return context;
  }

private:
  std::vector<std::shared_ptr<TextMapPropagator<T>>> propagators_;
};
}  // namespace propagation
}  // namespace trace
OPENTELEMETRY_END_NAMESPACE;