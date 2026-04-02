# OpenTelemetry C++ Explicit Parent Span Example

This example demonstrates how to explicitly set a parent span when the
thread-local active context cannot be relied upon. This is typical when
multiple spans coexist concurrently — for example, two parallel async tasks
each needing a different parent — so `GetCurrent()` cannot know which span
should be the parent of a new child.

## Running the example

Build and Deploy this opentelemetry-cpp example as described in [INSTALL.md](../../INSTALL.md).

## Auxiliary function

This example provides an auxiliary function that is able to create child
spans from a given parent, independently of the active span:

- `CreateSpan`: Creating Child Spans for a given explicit parent, or a root
  span when no parent is passed.

> It's worth noting that `shared_ptr`'s are used because it helps for keeping
> spans alive and passing them across different lambda functions or execution
> scopes for more complex use cases.

## Example Flow

- This example creates 2 parent spans (`parent_1`, `parent_2`) in the main
  thread.

- Each is passed to an independent `std::async` task, which creates a `nested`
  child span explicitly under that parent.

- The parent spans are kept alive until both tasks finish, ensuring the
  parent-child relationships are always valid.

## Span Hierarchy

Only 2 traces are generated in this example. Each one contains 2 spans.
The Span hierarchy is shown below:

```text
parent_1  (root)
└─→ nested

parent_2  (root)
└─→ nested
```

## Output

The output will be a set of spans, in which you can check how the relationships
outlined in the previous diagrams are fulfilled. For example:

### Trace 1

| name     | parent_span_id   | span_id          |
| -------- | ---------------- | ---------------- |
| parent_1 | 0000000000000000 | f026e45ec526047e |
| nested   | f026e45ec526047e | d701525271ff5d72 |

### Trace 2

| name     | parent_span_id   | span_id          |
| -------- | ---------------- | ---------------- |
| parent_2 | 0000000000000000 | a038787bb5eb6f1b |
| nested   | a038787bb5eb6f1b | 529c7df6581d9279 |
