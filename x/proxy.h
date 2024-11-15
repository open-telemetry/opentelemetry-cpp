struct ProxyOptions {
    bool enable_health_check{ true };
    bool enable_reflection{ true };
    bool insecure_credentials{ true };
    std::string listen_address{ "0.0.0.0:44317" };
    std::string forward_address{ "otel.ct.activision.com:4317" };

}