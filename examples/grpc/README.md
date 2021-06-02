# OpenTelemetry C++  Example

## gRPC

This is a simple example that demonstrates tracing a gRPC request from client to server. There is an experimental directory in this example - the code within has been commented out to prevent any conflicts. The example shows several aspects of tracing such as:

* Using the `TracerProvider`
* Implementing the TextMapCarrier
* Context injection/extraction
* Span Attributes
* Span Semantic Conventions
* Using the ostream exporter
* Nested spans
* W3c Trace Context Propagation (Very soon!)

### Running the example

1. The example uses gRPC C++ as well as Google's protocol buffers. Make sure you have
installed both of these packages on your system, in such a way that CMake would know
how to find them with this command:

```find_package(gRPC)```

If you install these with a package manager like '''brew''' or '''apt''', you should not need to do extra work.

2. Build and Deploy the opentelementry-cpp as described in [INSTALL.md](../../INSTALL.md). Building the project will build all of the examples and create new folders containing their executables within the 'build' directory NOT the 'examples' directory.

3. Start the server from your `build/examples/grpc` directory. Both the server and client are configured to use 8800 as the default port, but if you would like to use another port, you can specify that as an argument.

   ```console
   $ ./server [port_num]
    Server listening on port: 0.0.0.0:8800
   ```

4. In a separate terminal window, run the client to make a single request:

    ```console
    $ ./client [port_num]
    ```

5. You should see console exporter output for both the client and server sessions.
   * Client console

   ```console
   {
        name          : GreeterClient/Greet
        trace_id      : f898b8bca93936f112a56b710bb888eb
        span_id       : 8354bf34f7de06a0
        tracestate    :
        parent_span_id: 0000000000000000
        start         : 1620434679192923000
        duration      : 1571169
        description   :
        span kind     : Client
        status        : Ok
        attributes    :
                rpc.grpc.status_code: 0
                net.peer.port: 8800
                rpc.system: grpc
                net.peer.ip: 0.0.0.0
                rpc.method: Greet
                rpc.service: grpc-example.GreetService
        events        :
        links         :
   }
   ```

   * Server console

   ```console
   {
        name          : splitfunc
        trace_id      : 0836f1469dac75fddb57df13bcf33420
        span_id       : 97b3f69fae5c0e15
        tracestate    :
        parent_span_id: 3d5fcaccc5617153
        start         : 1620435268681957000
        duration      : 38026
        description   :
        span kind     : Internal
        status        : Unset
        attributes    :
        events        :
        links         :
    }

    {
        name          : splitlib
        trace_id      : 0836f1469dac75fddb57df13bcf33420
        span_id       : 3d5fcaccc5617153
        tracestate    :
        parent_span_id: a6aa48220c511354
        start         : 1620435268681937000
        duration      : 99145
        description   :
        span kind     : Internal
        status        : Unset
        attributes    :
        events        :
        links         :
    }

    {
        name          : GreeterService/Greet
        trace_id      : 0836f1469dac75fddb57df13bcf33420
        span_id       : a6aa48220c511354
        tracestate    :
        parent_span_id: 0000000000000000
        start         : 1620435268681893000
        duration      : 178531
        description   :
        span kind     : Serve
        status        : Ok
        attributes    :
                rpc.grpc.status_code: 0
                rpc.method: Greet
                rpc.service: GreeterService
                rpc.system: grpc
        events        :
        links         :
    }
   ```
