#include "ace/Method_Request.h"

ACE_BEGIN_VERSIONED_NAMESPACE_DECL

ACE_Method_Request::ACE_Method_Request (unsigned long prio)
  : priority_ (prio)
{
}

unsigned long
ACE_Method_Request::priority () const
{
  return this->priority_;
}

void
ACE_Method_Request::priority (unsigned long prio)
{
  this->priority_ = prio;
}

ACE_END_VERSIONED_NAMESPACE_DECL
