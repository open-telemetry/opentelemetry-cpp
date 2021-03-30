# OpenTelemetry C++  Example

## HTTP

This is a simple example that demonstrates tracing an HTTP request from client to server. The example shows several aspects of tracing such as:

* Using the `TracerProvider`
* Span Attributes
* Span Events
* Using the ostream exporter
* Nested spans (TBD)
* W3c Trace Context Propagation (TBD)

### Running the example

1. The example uses HTTP server and client provided as part of this repo:
    * [HTTP Client](https://github.com/open-telemetry/opentelemetry-cpp/tree/main/ext/include/opentelemetry/ext/http/client)
    * [HTTP Server](https://github.com/open-telemetry/opentelemetry-cpp/tree/main/ext/include/opentelemetry/ext/http/server)

2. Build and Deploy the opentelementry-cpp as described in [INSTALL.md](../../INSTALL.md)

3. Start the server from the `examples/http` directory

   ```console
   $ http_server 8800
    Server is running..Press ctrl-c to exit...
   ```

4. In a separate terminal window, run the client to make a single request:

    ```console
    $ ./http_client 8800
    ...
    ...
    ```

5. You should see console exporter output for both the client and server sessions.
   * Client console

   ```console
   {
     name          : /helloworld
     trace_id      : 15c7ca1993b536085f4097f2818a7be4
     span_id       : 7d9136e4eb4cb59d
     parent_span_id: 0000000000000000
     start         : 1617075613395810300
     duration      : 1901100
     description   :
     span kind     : Client
     status        : Unset
     attributes    :
           http.header.Date: Tue, 30 Mar 2021 03:40:13 GMT
           http.header.Content-Length: 0
           http.status_code: 200
           http.method: GET
           http.header.Host: localhost
           http.header.Content-Type: text/plain
           http.header.Connection: keep-alive
           http.scheme: http
           http.url: h**p://localhost:8800/helloworld
   }
   ```

   * Server console

   ```console
   {
     name          : /helloworld
     trace_id      : bfa611a4bbb8b1871ef6a222d6a0f4dd
     span_id       : 19e3cda7df63c9b9
     parent_span_id: 0000000000000000
     start         : 1617075522491536300
     duration      : 50700
     description   :
     span kind     : Server
     status        : Unset
     attributes    :
           http.header.Accept: */*
           http.request_content_length: 0
           http.header.Host: localhost:8800
           http.scheme: http
           http.client_ip: 127.0.0.1:44616
           http.method: GET
           net.host.port: 8800
           http.server_name: localhost
   }
   ```
