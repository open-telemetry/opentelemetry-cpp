#pragma once

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

class IRuntimeContext
{
public:

	// Provides a token with the passed in context
	Token CreateToken(Context context) noexcept
	{
		return Token(context);
	};

	virtual Context InternalGetCurrent() noexcept = 0;

	virtual Token InternalAttach(Context context) noexcept = 0;

	virtual bool InternalDetach(Token &token) noexcept = 0;
};

}  // namespace context
OPENTELEMETRY_END_NAMESPACE
