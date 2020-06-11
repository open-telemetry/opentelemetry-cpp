#pragma once

#include <bits/stdc++.h>
#include <atomic>
#include <map>
#include <vector>

#include "opentelemetry/nostd/string_view.h"
#include "opentelemetry/nostd/variant.h"
#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

/*The context class provides a context identifier */
class Context
{

public:
  /*The Key class is used to obscure access from the
   * user to the context map. The identifier is used as a key
   * to the context map.
   */
  class Key
  {
  private:
    friend class Context;

    nostd::string_view key_name_;

    Key* identifier_;

    /* GetIdentifier: returns the identifier */
    Key* GetIdentifier() { return identifier_; }

    /* Constructs a new Key with the passed in name and
     * identifier.
     */
    Key(nostd::string_view key_name, int identifier)
    {
      key_name_   = key_name;
      identifier_ = this;
    }

  public:
    /* Consructs a new Key with the passed in name and increments
     * the identifier then assigns it to be the key's identifier.
     */
    Key(nostd::string_view key_name)
    {
      key_name_ = key_name;
      identifier_ = this;
    }
  };

  /* Creates a context object with no key/value pairs */
  Context() = default;

  /* Contructor, creates a context object from a map
   * of keys and identifiers
   */
  Context(Key key, int value) { ctx_map_[key.GetIdentifier()] = value; }

  /* Accepts a new key/value pair and then returns a new
   * context that contains both the original pairs and the new pair.
   */
  Context WriteValue(Key key, int value)
  {
    std::map<Key*, int> temp_map = ctx_map_;
    temp_map[key.GetIdentifier()] = value;
    return Context(temp_map);
  }

  /* Accepts a vector of key value pairs and combines them with the
   * existing context map then returns a new context with the new
   * combined map */
  Context WriteValues(std::vector<std::pair<Key, int>> ctx_list)
  {
    std::map<Key*, int> temp_map = ctx_map_;
    
    for (auto i = ctx_list.begin(); i != ctx_list.end(); i++)
    {
      temp_map[((*i).first).GetIdentifier()] = (*i).second;
    }

    return Context(temp_map);
  }

  /* Class comparator to see if the context maps are the same. */
  bool operator==(const Context &context)
  {
    if (context.ctx_map_ == ctx_map_)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  /* Returns the value associated with the passed in key */
  int GetValue(Key key) { return ctx_map_[key.GetIdentifier()]; }

  /* Returns a Key that has the passed in name and the
   * next available identifier.*/
  Key CreateKey(nostd::string_view key_name)
  {
    return Key(key_name);
  }

  /* Copy constructors */
  Context(const Context &other) = default;
  Context &operator=(const Context &other) = default;

private:
  /* The identifier itself */
  std::map<Key*, int> ctx_map_;

  /* A constructor that accepts a key/value map */
  Context(std::map<Key*, int> ctx_map) { ctx_map_ = ctx_map; }
};

/* The RuntimeContext class provides a wrapper for
 * propogating context through cpp. */
class RuntimeContext
{

public:
  /* The token class provides an identifier that is used by
   * the attach and detach methods to keep track of context
   * objects.
   */

  class Token
  {

  private:
    friend class RuntimeContext;

    Context ctx_;

    /* A constructor that sets the token's Context object to the
     * one that was passed in.
     */
    Token(Context &ctx) { ctx_ = ctx; }

    /* Returns the stored context object */
    Context GetContext() { return ctx_; }
  };

  /* A default constructor that will set the context to
   * an empty context object.
   */
  RuntimeContext() { context_ = Context(); }

  /* A constructor that will set the context as the passed in context. */
  RuntimeContext(Context &context) { context_ = context; }

  /* Sets the current 'Context' object. Returns a token
   * that can be used to reset to the previous Context.
   */
  Token Attach(Context &context)
  {

    Token old_context_token = Token(context_);

    context_ = context;

    return old_context_token;
  }

  /* Return the current context. */
  static Context GetCurrent()
  {
    Context context = context_;
    return context_;
  }

  /* Resets the context to a previous value stored in the
   * passed in token. Returns zero if successful, -1 otherwise
   */
  int Detach(Token &token)
  {

    if (token.GetContext() == context_)
    {

      return -1;
    }

    context_ = token.GetContext();

    return 0;
  }

private:
  static thread_local Context context_;
};

thread_local Context RuntimeContext::context_ = Context();
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
