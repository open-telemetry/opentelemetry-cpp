# OpenTelemetry C++  Example

## HTTP

This is a simple example that demonstrates tracing an HTTP request from client to server. The example shows several aspects of tracing, such as:

* Using the `TracerProvider`
* Span Attributes
* Using the console exporter

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
     name          : sentRequest
     trace_id      : a70a75ade26d2c1fed6cfd5116f4a3db
     span_id       : ed870ae26eb1b78c
     parent_span_id: 0000000000000000
     start         : 1616754223528238500
     duration      : 1848300
     description   :
     span kind     : Client
     status        : Unset
     attributes    :
           http.session_state: 8
           http.header.Date: Fri, 26 Mar 2021 10:23:43 GMT
           http.header.Content-Length: 0
           http.status_code: 200
           http.header.Host: localhost
           http.header.Content-Type: text/plain
           http.header.Connection: keep-alive
           http.url: http://localhost:8800/helloworld
           http.method: GET
           component: http
   }

   ```

   * Server console

   ```console
   {
     name          : handle_request
     trace_id      : f92ff8f5f6553ead210f2643779e1587
     span_id       : 195fae1770f514ce
     parent_span_id: 0000000000000000
     start         : 1616754460952018400
     duration      : 20600
     description   :
     span kind     : Server
     status        : Unset
     attributes    :
           http.req.headerHost: localhost:8800
           http.req.headerAccept: */*
           component: http
           req_method: GET
           req_url: /helloworld
   }
   ```
