Getting started
---------------

Tracing
~~~~~~~

When instrumenting libraries and applications, the most simple approach
requires three steps.

Obtain a tracer
^^^^^^^^^^^^^^^

.. code:: cpp

    auto provider = opentelemetry::trace::Provider::GetTracerProvider();
    auto tracer = provider->GetTracer("foo_library", "1.0.0");

The ``TracerProvider`` acquired in the first step is a singleton object
that is usually provided by the OpenTelemetry C++ SDK. It is used to
provide specific implementations for API interfaces. In case no SDK is
used, the API provides a default no-op implementation of a
``TracerProvider``.

The ``Tracer`` acquired in the second step is needed to create and start
``Span``\ s.

Start a span
^^^^^^^^^^^^

.. code:: cpp

    auto span = tracer->StartSpan("HandleRequest");

This creates a span, sets its name to ``"HandleRequest"``, and sets its
start time to the current time. Refer to the API documentation for other
operations that are available to enrich spans with additional data.

Mark a span as active
^^^^^^^^^^^^^^^^^^^^^

.. code:: cpp

    auto scope = tracer->WithActiveSpan(span);

This marks a span as active and returns a ``Scope`` object bound to the
lifetime of the span. When the ``Scope`` object is destroyed, the
related span is ended.

The concept of an active span is important, as any span that is created
without explicitly specifying a parent is parented to the currently
active span.

