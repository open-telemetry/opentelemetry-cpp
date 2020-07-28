#pragma once

#include "opentelemetry/context/context.h"
#include "opentelemetry/context/runtime_context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{

// The ThreadLocalContext class is a derived class from RuntimeContext and
// provides a wrapper for propogating context through cpp thread locally.
// This file must be included to use the RuntimeContext class if another
// implementation has not been registered.
class ThreadLocalContext : public RuntimeContext
{
public:
  ThreadLocalContext() noexcept = default;

  // Return the current context.
  Context InternalGetCurrent() noexcept override { return stack_.Top(); }

  // Resets the context to a previous value stored in the
  // passed in token. Returns true if successful, false otherwise
  bool InternalDetach(Token &token) noexcept override
  {
    if (!(token == stack_.Top()))
    {
      return false;
    }
    stack_.Pop();
    return true;
  }

  // Sets the current 'Context' object. Returns a token
  // that can be used to reset to the previous Context.
  Token InternalAttach(Context context) noexcept override
  {
    stack_.Push(context);
    Token old_context = CreateToken(context);
    return old_context;
  }

private:
  // A nested class to store the attached contexts in a stack.
  class Stack
  {
    friend class ThreadLocalContext;

    Stack() noexcept : size_(0), capacity_(0), base_(nullptr){};

    // Pops the top Context off the stack and returns it.
    Context Pop() noexcept
    {
      if (size_ <= 0)
      {
        return Context();
      }
      int index = size_ - 1;
      size_--;
      return base_[index];
    }

    // Returns the Context at the top of the stack.
    Context Top() const noexcept
    {
      if (size_ <= 0)
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

  static thread_local Stack stack_;
};
thread_local ThreadLocalContext::Stack ThreadLocalContext::stack_ = ThreadLocalContext::Stack();

// Registers the ThreadLocalContext as the context handler for the RuntimeContext
RuntimeContext *RuntimeContext::context_handler_ = new ThreadLocalContext();
}  // namespace context
OPENTELEMETRY_END_NAMESPACE
