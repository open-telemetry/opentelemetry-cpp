#pragma once

#include "opentelemetry/trace/key_value_iterable.h"
#include "opentelemetry/context/context.h"
#include "opentelemetry/version.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/common/attribute_value.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{


    template <class T>
    class KeyValueIterableModifiable final : public trace::KeyValueIterable
  {
    //static_assert(trace::detail::is_key_value_iterable<T>::value, "Must be a key-value iterable");

    public:
      KeyValueIterableModifiable(T container) noexcept : container_{container} {}

      KeyValueIterableModifiable() = default;
      // KeyValueIterable
      void PrintKeys()
      {
        std::cout << "Printing Keys" << std::endl;
        auto iter = std::begin(container_);
        auto last = std::end(container_);
        for (; iter != last; ++iter)
        {
          std::cout /*<< iter->first << " "*/ << iter->second << std::endl;
        }
        //        return true;
      }
      
      void Add(T container)
      {
          
         std::insert_iterator<T> back (container_, std::begin(container_));
          
          auto iter = std::begin(container);
          auto last = std::end(container);
          for (; iter != last; ++iter)
          {
            back = *iter;
          }
      
      }
      
      template <typename K> 
      common::AttributeValue Get(K key)
      {
          auto iter = std::begin(container_);
          auto last = std::end(container_);
          for (; iter != last; ++iter)
          {
            
            if(iter->first == key){
              return iter->second;
            } 
                          
          }

          return "";
          
      
      }

      T GetData(){
        return container_;
      }

      // KeyValueIterable
      bool ForEachKeyValue(
          nostd::function_ref<bool(nostd::string_view, common::AttributeValue)> callback) const
        noexcept override
        {
          auto iter = std::begin(container_);
          auto last = std::end(container_);
          for (; iter != last; ++iter)
          {
/*            if (!callback(iter->first, iter->second))
            {
              return false;
            }*/
          }
          return true;
        }

      size_t size() const noexcept override { return nostd::size(container_); }



    private:
      T container_;
  };
}  // namespace nostd
OPENTELEMETRY_END_NAMESPACE
