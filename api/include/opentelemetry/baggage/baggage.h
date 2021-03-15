#include "opentelemetry/common/kv_properties.h"
#include "opentelemetry/nostd/shared_ptr.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace baggage
{

class Baggage
{

public:
  Baggage() : kv_properties_(new opentelemetry::common::KeyValueProperties()) {}

  template <class T>
  Baggage(const T &keys_and_values)
      : kv_properties_(new opentelemetry::common::KeyValueProperties(T))
  {}

  bool GetValue(const nostd::string_view &key, std::string &value)
  {
    return kv_properties_->GetValue(key, value);
  }

  // @return all key-values entris by repeatedly invoking the function reference passed as argument
  // for each entry
  bool GetAllEntries(
      nostd::function_ref<bool(nostd::string_view, nostd::string_view)> callback) const noexcept
  {
    return kv_properties_->GetAllEntries(
        callback);  // [&callback](nostd::string_view key, nostd::string_view value) {
  }

  nostd::shared_ptr<Baggage> Delete(const nostd::string_view &key)
  {
    nostd::shared_ptr<Baggage> baggage(new Baggage(kv_properties_->Size() - 1));
    kv_properties_->GetAllEntries(
        [&baggage, &key](nostd::string_view e_key, nostd::string_view e_value) {
          if (key != e_key)
            baggage->kv_properties_->AddEntry(e_key, e_value);
          return true;
        });
    return baggage;
  }

  nostd::shared_ptr<Baggage> Set(const nostd::string_view &key, const nostd::string_view &value)
  {
    nostd::shared_ptr<Baggage> baggage(new Baggage(kv_properties_->Size() + 1));
    baggage->kv_properties_->AddEntry(key, value);
    // add rest of the fields.
    kv_properties_->GetAllEntries([&baggage](nostd::string_view key, nostd::string_view value) {
      baggage->kv_properties_->AddEntry(key, value);
      return true;
    });
    return baggage;
  }

private:
  // Store entries in a C-style array to avoid using std::array or std::vector.

  nostd::unique_ptr<opentelemetry::common::KeyValueProperties> kv_properties_;
  Baggage(size_t size) : kv_properties_(new opentelemetry::common::KeyValueProperties(size)){};
};
}  // namespace baggage
OPENTELEMETRY_END_NAMESPACE