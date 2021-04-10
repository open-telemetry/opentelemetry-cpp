## OpenTelemetry C++ Example

Grpc
This is a simple example that demonstrates tracing an request using grpc that was done under user reasearch

The example tests the auto instrumentations and spans.

The example uses GRPC server and client provided as part of this repo:

greeter_server.cc
greeter_client.cc

Build and Deploy the opentelementry-cpp as described in INSTALL.md

The server sleeps for 30ms then writes HelloWorld. On the server, configure the SDK to only batch a small number of spans, and to flush them quickly, 
so that you can see your data without having to wait. The default settings of the OpenTelemetry SDK achieve this, so we do not need explicitly make changes

Start the server from the examples/grpc directory

$ ./greeter_server
 Server listening on 0.0.0.0:50051.
In a separate terminal window, run the client to make a single request:

$ ./greeter_client
...
...
You should see a Hello World message on the console.

You should also be able to traces created as a part of auto instrumentation
