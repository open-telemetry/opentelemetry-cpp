/* this file will be removed once we have unit test and CI */

#include <stdio.h>

#include "traceparent.h"
#include "tracestate.h"

int main(int argc, char* argv[])
{
  TraceParent trace_parent;
  status_t retval = trace_parent_from_string(&trace_parent, "00-12345678901234567890123456789012-1234567890123456-01");
  printf("Hello, World! %d\n", retval);
  return 0;
}
