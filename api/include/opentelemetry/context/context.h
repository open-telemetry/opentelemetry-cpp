#pragma once

#include <atomic>
#include <iostream>
#include <sstream>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/utility.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/trace/key_value_iterable.h"
#include "opentelemetry/trace/key_value_iterable_view.h"
#include "opentelemetry/trace/tracer.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

// The context class provides a context identifier.
class Context
{

public:

  // The Key class is used to obscure access from the
  // user to the context map. The identifier is used as a key
  // to the context map.
  class Key
  {

  public:
    
    // Returns the key's identifier.
    nostd::string_view GetIdentifier() { return nostd::string_view(identifier_); }
    
    //Returns the key's name.
    nostd::string_view GetName() { return key_name_; }

    Key(const Key &other){
      key_name_ = other.key_name_;
      memcpy (identifier_, other.identifier_, 50);
    }

  private:
    friend class Context;

    // Constructs a new Key with the passed in name. Sets the identifier as
    // the address of this object.
    Key(nostd::string_view key_name) : key_name_{key_name}
    {
      std::stringstream ss;
      ss << (void *)this;
      nostd::string_view temp_view;
      temp_view = ss.str();

      memcpy(identifier_, temp_view.data(), temp_view.size());
    }

    nostd::string_view key_name_;
    
    char name_[50];
    
    char identifier_[50];
  };
  
  // A linked list to contain the keys and values of this context node 
  class DataList {
    private:
      friend class Context;
 
      DataList *next;
      
      char key_[50];
      
      common::AttributeValue value_;
  };
  

  // Creates a key with the passed in name and returns it.
  static Key CreateKey(nostd::string_view key_name) {
    Key *master = new Key(key_name); 
    return  Key(*master); 
  }
  
  Context(){
    next_ = NULL;
    head_ = NULL;
  }

  // Creates a context object from a map of keysand identifiers.
  template <class T , nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
  Context(const T &attributes)  
  {
    auto iter = std::begin(attributes);
    
    DataList *head = new DataList;
    strcpy(head->key_, nostd::string_view(iter->first).data());
    head->value_ = iter->second;
    ++iter;

    DataList *previous_node = head;
    for (; iter != std::end(attributes); ++iter)
    {
      DataList *node = new DataList;
      
      node->next = NULL;
      strcpy(node->key_, nostd::string_view(iter->first).data());
      node->value_ = iter->second;
      
      previous_node->next = node;
      previous_node = node;
    }
     
    head_ = head;
  }

  // Accepts a new iterable and then returns a new context that
  // contains the new key and value data.
  template <class T , nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
  Context WriteValues(T &attributes) noexcept
  {
    Context *master = new Context(attributes);
    master->next_ = this;  
    return Context(*master);
  }

  // Returns the value associated with the passed in key.
  common::AttributeValue GetValue(Key key)
  {
    Context *context_trace = this;

    while(context_trace != NULL){
      DataList *data_trace = context_trace->head_;
      
      while(data_trace != NULL){
        if(strncmp(key.GetIdentifier().data(), data_trace->key_, 50) == 0){
          return data_trace->value_;
        }
        data_trace = data_trace->next;
      }
      
      context_trace = context_trace->next_;
    }
    
    return "";
  }

  // Iterates through the current and comparing context objects
  // to check for equality,
  bool operator==(const Context &other)
  {
    if(next_ != other.next_){
      return false; 
    }
    
    DataList *trace = head_;
    DataList *other_trace = other.head_;
    while(trace != NULL){

      if(other_trace == NULL){
        return false;
      }
      
      // TODO add value comparison
      if((trace->key_ != other_trace->key_) /*|| !((common::AttributeValue)trace->value_ == (common::AttributeValue)other_trace->value_)*/){
        return false;
      }
      trace = trace->next;
      other_trace = other_trace->next;
    }

    return true;
  }

  Context(const Context &other) = default;
  
  Context &operator=(const Context &other){
    head_ = other.head_;
    next_ = other.next_;
  }

private:
  
  // Head of the list which holds the keys and values of this context 
  DataList *head_;
  
  // Pointer to the next context object in the context list
  Context *next_;
};

}  // namespace context
OPENTELEMETRY_END_NAMESPACE
