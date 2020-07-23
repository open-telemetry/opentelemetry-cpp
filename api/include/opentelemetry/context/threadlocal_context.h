#pragma once

#include "opentelemetry/context/runtime_context.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace context
{
// The ThreadLocalContext class is a derived class from RuntimeContext and
// provides a wrapper for propogating context through cpp thread locally.
class ThreadLocalContext
{
public:
  ThreadLocalContext() {}

  // The token class provides an identifier that is used by
  // the attach and detach methods to keep track of context
  // objects.
  class Token
  {
  private:
    friend class ThreadLocalContext;

    Context *context_;

    // A constructor that sets the token's Context object to the
    // one that was passed in.
    Token(Context *context) { context_ = context; }

    // Returns the stored context object.
    Context *GetContext() { return context_; }
  };

  // Return the current context.
  Context *GetCurrent() { return stack_.Top(); }

  // Resets the context to a previous value stored in the
  // passed in token. Returns true if successful, false otherwise
  bool Detach(Token &token)
  {

    if (!(token.GetContext() == stack_.Top()))
    {
      return false;
    }

    stack_.Pop();
    return true;
  }

  // Sets the current 'Context' object. Returns a token
  // that can be used to reset to the previous Context.
  Token Attach(Context *context)
  {
    stack_.Push(context);
    Token old_context = Token(context);
    return old_context;
  }

private:
  // A nested class to store the attached contexts in a stack.
  class Stack
  {
    friend class ThreadLocalContext;

    Stack()
    {
      size_     = 0;
      capacity_ = 0;
      base_     = nullptr;
    }

    // Pops the top Context* off the stack and returns it.
    Context *Pop()
    {
      if (size_ <= 0)
      {
        return nullptr;
      }
      int index = size_ - 1;
      size_--;
      return base_[index];
    }

    // Returns the Context* at the top of the stack.
    Context *Top()
    {
      if (size_ <= 0)
      {
        return nullptr;
      }
      return base_[size_ - 1];
    }

    // Pushes the passed in context pointer to the top of the stack
    // and resizes if necessary.
    void Push(Context *context)
    {
      size_++;
      if (size_ > capacity_)
      {
        Resize(size_ * 2);
      }
      base_[size_ - 1] = context;
    }

    // Reallocates the storage array to the pass in new capacity size.
    void Resize(int new_capacity)
    {
      int old_size = size_ - 1;
      if (new_capacity == 0)
      {
        new_capacity = 2;
      }
      Context **temp = new Context *[new_capacity];
      memcpy(temp, base_, sizeof(Context *) * old_size);
      delete[] base_;
      base_ = temp;
    }

    ~Stack() { delete[] base_; }

    int size_;
    int capacity_;
    Context **base_;
  };

  static thread_local Stack stack_;
};
thread_local ThreadLocalContext::Stack ThreadLocalContext::stack_ = ThreadLocalContext::Stack();

}  // namespace context
OPENTELEMETRY_END_NAMESPACE
