#pragma once

#include <atomic>
#include <iostream>
#include <sstream>

#include "opentelemetry/common/attribute_value.h"
#include "opentelemetry/context/context_value.h"
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

// The context class provides a context identifier. Is built as a linked list
// of context nodes which each have access to the nodes before it, but not 
// after. So each time a write is performed a new context is added to the list.
// Within each context node there is another linked list which holds the key
// and value data for each node.
class Context
{

public:
  
  // A linked list to contain the keys and values of this context node 
  class DataList {
    private:
      friend class Context;
 
      DataList *next_;
      
      char key_[50];
      
      ContextValue value_;
  };
  
  Context(){
    next_ = NULL;
    head_ = NULL;
  }

  // Creates a context object from a map of keys and identifiers, this will
  // be the head of the context object linked list.
  template <class T , nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
  Context(const T &keys_and_values)  
  {
    head_ = BuildList(keys_and_values);
    next_ = NULL;
  }
  
  // Creates a context object from a key and value, this will be the head
  // of the context object linked list.
  Context(nostd::string_view key, ContextValue value)  
  {
    head_ = BuildHead(key, value);
    next_ = NULL;
  }

  // Accepts a new iterable and then returns a new context that
  // contains the new key and value data.
  template <class T , nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
  Context* SetValues(T &values) noexcept
  {
    Context *new_node = new Context(values, this);
    return new_node;
  }
  
  // Accepts a new iterable and then returns a new context that
  // contains the new key and value data.
  Context* SetValue(nostd::string_view key, ContextValue value) noexcept
  {
    Context *new_node = new Context(key,value, this);
    return new_node;
  }

  // Returns the value associated with the passed in key.
  common::AttributeValue GetValue(nostd::string_view key)
  {
    Context *context_trace = this;
    
    //Iterate through the context nodes
    while(context_trace != NULL){
      DataList *data_trace = context_trace->head_;
      
      //Iterate through the internal data nodes
      while(data_trace != NULL){
        if(strncmp(key.data(), data_trace->key_, 50) == 0){
          return data_trace->value_;
        }
        
        data_trace = data_trace->next_;
      }
      
      context_trace = context_trace->next_;
    }
    
    return "";
  }

  // Iterates through the current and comparing context objects
  // to check for equality.
  bool operator==(const Context &other)
  {
    // If the two contexts are not at the place in the list
    // they cannot be equivalent
    if(next_ != other.next_){
      return false; 
    }
    
    DataList *trace = head_;
    DataList *other_trace = other.head_;
    
    // Iterate through the lists and compare the keys
    // TODO should also compare the values but the nostd::variant
    // seems to not have a comparison operator
    while(trace != NULL){

      if(other_trace == NULL){
        return false;
      }
      
      if((trace->key_ != other_trace->key_)){
        return false;
      }

      trace = trace->next_;
      other_trace = other_trace->next_;
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
  
  // Builds a data list off of a key and value iterable and returns the head 
  template <class T , nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
  DataList* BuildList(const T &keys_and_vals){
    auto iter = std::begin(keys_and_vals);
    
    // Create list head 
    DataList *head = new DataList;
    strcpy(head->key_, nostd::string_view(iter->first).data());
    head->value_ = iter->second;
    head->next_ = NULL;
    ++iter;
    
    DataList *previous_node = head;
    // Iterate over the keys and values iterable and add nodes
    for (; iter != std::end(keys_and_vals); ++iter)
    {
      DataList *node = new DataList;
      node->next_ = NULL;
      strcpy(node->key_, nostd::string_view(iter->first).data());
      node->value_ = iter->second;
      previous_node->next_ = node;
      previous_node = node;
    }

    return head; 
  }  
  
  // Builds a data list with just a key and value, so it will just be the head
  // and returns that head. 
  DataList* BuildHead(nostd::string_view key, ContextValue value){
    DataList *head = new DataList;
    strcpy(head->key_, key.data());
    head->value_ = value;
    head->next_ = NULL;
    return head; 
  }  
  
  
  // Creates a context object from a map of keys and identifiers, an internal 
  // contructor only, will always create a node within the list, not at the 
  // head.
  template <class T , nostd::enable_if_t<trace::detail::is_key_value_iterable<T>::value> * = nullptr>
  Context(const T &attributes, Context* next_node)  
  {
    head_ = BuildList(attributes);
    next_ = next_node;
  }
  
  // Creates a context object from a key and value. An internal 
  // contructor only, will always create a node within the list, not at the 
  // head.
  Context(nostd::string_view key, ContextValue value, Context* next_node)  
  {
    head_ = BuildHead(key, value);
    next_ = next_node;
  }
};

}  // namespace context
OPENTELEMETRY_END_NAMESPACE
