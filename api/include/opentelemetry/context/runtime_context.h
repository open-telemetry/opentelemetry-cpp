#pragma once

#include <atomic>

#include "opentelemetry/context/context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{
// The Token object provides is returned when attaching objects to the
// RuntimeContext object and is associated with a context object, and
// can be provided to the RuntimeContext Detach method to remove the
// associated context from the RuntimeContext.
class Token
{
public:
  bool operator==(const Context &other) noexcept { return context_ == other; }

private:
  friend class RuntimeContextStorage;

  // The ContextDetacher object automatically attempts to detach
  // the Token when all copies of the Token are out of scope.
  class ContextDetacher
  {
  public:
    ContextDetacher(Context context) : context_(context) {}

    ~ContextDetacher();

  private:
    Context context_;
  };

  Token() noexcept = default;

  // A constructor that sets the token's Context object to the
  // one that was passed in.
  Token(Context context)
  {
    context_ = context;

    detacher_ = nostd::shared_ptr<ContextDetacher>(new ContextDetacher(context_));
  };

  Context context_;
  nostd::shared_ptr<ContextDetacher> detacher_;
};

/**
 * RuntimeContextStorage is used by RuntimeContext to store Context frames.
 *
 * Custom context management strategies can be implemented by deriving from
 * this class and passing an initialized RuntimeContextStorage object to
 * RuntimeContext::SetRuntimeContextStorage.
 */
class RuntimeContextStorage
{
public:
  /**
   * Return the current context.
   * @return the current context
   */
  virtual Context GetCurrent() noexcept = 0;

  /**
   * Set the current context.
   * @param the new current context
   */
  virtual Token Attach(Context context) noexcept = 0;

  /**
   * Detach the context related to the given token.
   * @param token a token related to a context
   * @return true if the context could be detached
   */
  virtual bool Detach(Token &token) noexcept = 0;

protected:
  Token CreateToken(Context context) noexcept { return Token(context); }
};

/**
 * Construct and return the default RuntimeContextStorage
 * @return a ThreadLocalContextStorage
 */
static RuntimeContextStorage *GetDefaultStorage() noexcept;

// Provides a wrapper for propagating the context object globally.
//
// By default, a thread-local runtime context storage is used.
class RuntimeContext
{
public:
  // Return the current context.
  static Context GetCurrent() noexcept { return GetRuntimeContextStorage()->GetCurrent(); }

  // Sets the current 'Context' object. Returns a token
  // that can be used to reset to the previous Context.
  static Token Attach(Context context) noexcept
  {
    return GetRuntimeContextStorage()->Attach(context);
  }

  // Resets the context to a previous value stored in the
  // passed in token. Returns true if successful, false otherwise
  static bool Detach(Token &token) noexcept { return GetRuntimeContextStorage()->Detach(token); }

  // Sets the Key and Value into the passed in context or if a context is not
  // passed in, the RuntimeContext.
  // Should be used to SetValues to the current RuntimeContext, is essentially
  // equivalent to RuntimeContext::GetCurrent().SetValue(key,value). Keep in
  // mind that the current RuntimeContext will not be changed, and the new
  // context will be returned.
  static Context SetValue(nostd::string_view key,
                          ContextValue value,
                          Context *context = nullptr) noexcept
  {
    Context temp_context;
    if (context == nullptr)
    {
      temp_context = GetCurrent();
    }
    else
    {
      temp_context = *context;
    }
    return temp_context.SetValue(key, value);
  }

  // Returns the value associated with the passed in key and either the
  // passed in context* or the runtime context if a context is not passed in.
  // Should be used to get values from the current RuntimeContext, is
  // essentially equivalent to RuntimeContext::GetCurrent().GetValue(key).
  static ContextValue GetValue(nostd::string_view key, Context *context = nullptr) noexcept
  {
    Context temp_context;
    if (context == nullptr)
    {
      temp_context = GetCurrent();
    }
    else
    {
      temp_context = *context;
    }
    return temp_context.GetValue(key);
  }

  /**
   * Provide a custom runtime context storage.
   *
   * This provides a possibility to override the default thread-local runtime
   * context storage. This has to be set before any spans are created by the
   * application, otherwise context state information will be discarded, which
   * might lead to unexpected results.
   *
   * @param storage a custom runtime context storage
   */
  static void SetRuntimeContextStorage(nostd::shared_ptr<RuntimeContextStorage> storage) noexcept
  {
    while (GetLock().test_and_set(std::memory_order_acquire))
      ;
    GetStorage() = storage;
    GetLock().clear(std::memory_order_release);
  }

private:
  static nostd::shared_ptr<RuntimeContextStorage> GetRuntimeContextStorage() noexcept
  {
    while (GetLock().test_and_set(std::memory_order_acquire))
      ;
    auto storage = nostd::shared_ptr<RuntimeContextStorage>(GetStorage());
    GetLock().clear(std::memory_order_release);

    return storage;
  }

  static nostd::shared_ptr<RuntimeContextStorage> &GetStorage() noexcept
  {
    static nostd::shared_ptr<RuntimeContextStorage> context(GetDefaultStorage());
    return context;
  }

  static std::atomic_flag &GetLock() noexcept
  {
    static std::atomic_flag lock = ATOMIC_FLAG_INIT;
    return lock;
  }
};

inline Token::ContextDetacher::~ContextDetacher()
{
  context::Token token;
  token.context_ = context_;
  context::RuntimeContext::Detach(token);
}

// The ThreadLocalContextStorage class is a derived class from RuntimeContext and
// provides a wrapper for propogating context through cpp thread locally.
// This file must be included to use the RuntimeContext class if another
// implementation has not been registered.
class ThreadLocalContextStorage : public RuntimeContextStorage
{
public:
  ThreadLocalContextStorage() noexcept = default;

  // Return the current context.
  Context GetCurrent() noexcept override { return GetStack().Top(); }

  // Resets the context to a previous value stored in the
  // passed in token. Returns true if successful, false otherwise
  bool Detach(Token &token) noexcept override
  {
    if (!(token == GetStack().Top()))
    {
      return false;
    }
    GetStack().Pop();
    return true;
  }

  // Sets the current 'Context' object. Returns a token
  // that can be used to reset to the previous Context.
  Token Attach(Context context) noexcept override
  {
    GetStack().Push(context);
    Token old_context = CreateToken(context);
    return old_context;
  }

private:
  // A nested class to store the attached contexts in a stack.
  class Stack
  {
    friend class ThreadLocalContextStorage;

    Stack() noexcept : size_(0), capacity_(0), base_(nullptr){};

    // Pops the top Context off the stack and returns it.
    Context Pop() noexcept
    {
      if (size_ == 0)
      {
        return Context();
      }
      size_ -= 1;
      return base_[size_];
    }

    // Returns the Context at the top of the stack.
    Context Top() const noexcept
    {
      if (size_ == 0)
      {
        return Context();
      }
      return base_[size_ - 1];
    }

    // Pushes the passed in context pointer to the top of the stack
    // and resizes if necessary.
    void Push(Context context) noexcept
    {
      size_++;
      if (size_ > capacity_)
      {
        Resize(size_ * 2);
      }
      base_[size_ - 1] = context;
    }

    // Reallocates the storage array to the pass in new capacity size.
    void Resize(int new_capacity) noexcept
    {
      int old_size = size_ - 1;
      if (new_capacity == 0)
      {
        new_capacity = 2;
      }
      Context *temp = new Context[new_capacity];
      if (base_ != nullptr)
      {
        std::copy(base_, base_ + old_size, temp);
        delete[] base_;
      }
      base_ = temp;
    }

    ~Stack() noexcept { delete[] base_; }

    size_t size_;
    size_t capacity_;
    Context *base_;
  };

  Stack &GetStack()
  {
    static thread_local Stack stack_ = Stack();
    return stack_;
  }
};

static RuntimeContextStorage *GetDefaultStorage() noexcept
{
  return new ThreadLocalContextStorage();
}
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
