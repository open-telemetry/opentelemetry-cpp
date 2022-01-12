load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def opentelemetry_cpp_deps():
    """Loads dependencies need to compile the opentelemetry-cpp library."""

    # Google Benchmark library.
    # Only needed for benchmarks, not to build the OpenTelemetry library.
    maybe(
        http_archive,
        name = "com_github_google_benchmark",
        sha256 = "1f71c72ce08d2c1310011ea6436b31e39ccab8c2db94186d26657d41747c85d6",
        strip_prefix = "benchmark-1.6.0",
        urls = [
            "https://github.com/google/benchmark/archive/v1.6.0.tar.gz",
        ],
    )

    # GoogleTest framework.
    # Only needed for tests, not to build the OpenTelemetry library.
    maybe(
        http_archive,
        name = "com_google_googletest",
        sha256 = "a03a7b24b3a0766dc823c9008dd32c56a1183889c04f084653266af22289ab0c",
        strip_prefix = "googletest-a6dfd3aca7f2f91f95fc7ab650c95a48420d513d",
        urls = [
            "https://github.com/google/googletest/archive/a6dfd3aca7f2f91f95fc7ab650c95a48420d513d.tar.gz",
        ],
    )

    # Load gRPC dependency
    maybe(
        http_archive,
        name = "com_github_grpc_grpc_legacy",
        sha256 = "024118069912358e60722a2b7e507e9c3b51eeaeee06e2dd9d95d9c16f6639ec",
        strip_prefix = "grpc-1.39.1",
        urls = [
            "https://github.com/grpc/grpc/archive/v1.39.1.tar.gz",
        ],
    )

    maybe(
        http_archive,
        name = "com_github_grpc_grpc",
        sha256 = "024118069912358e60722a2b7e507e9c3b51eeaeee06e2dd9d95d9c16f6639ec",
        strip_prefix = "grpc-1.39.1",
        urls = [
            "https://github.com/grpc/grpc/archive/v1.39.1.tar.gz",
        ],
    )

    # OTLP Protocol definition
    maybe(
        http_archive,
        name = "com_github_opentelemetry_proto",
        build_file = "@io_opentelemetry_cpp//bazel:opentelemetry_proto.BUILD",
        sha256 = "985367f8905e91018e636cbf0d83ab3f834b665c4f5899a27d10cae9657710e2",
        strip_prefix = "opentelemetry-proto-0.11.0",
        urls = [
            "https://github.com/open-telemetry/opentelemetry-proto/archive/v0.11.0.tar.gz",
        ],
    )

    # JSON library
    maybe(
        http_archive,
        name = "github_nlohmann_json",
        build_file = "@io_opentelemetry_cpp//bazel:nlohmann_json.BUILD",
        sha256 = "69cc88207ce91347ea530b227ff0776db82dcb8de6704e1a3d74f4841bc651cf",
        urls = [
            "https://github.com/nlohmann/json/releases/download/v3.6.1/include.zip",
        ],
    )

    # C++ Prometheus Client library.
    maybe(
        http_archive,
        name = "com_github_jupp0r_prometheus_cpp",
        sha256 = "aab4ef8342319f631969e01b8c41e355704847cbe76131cb1dd5ea1862000bda",
        strip_prefix = "prometheus-cpp-0.11.0",
        urls = [
            "https://github.com/jupp0r/prometheus-cpp/archive/v0.11.0.tar.gz",
        ],
    )

    # libcurl (optional)
    maybe(
        http_archive,
        name = "curl",
        build_file = "@io_opentelemetry_cpp//bazel:curl.BUILD",
        sha256 = "ba98332752257b47b9dea6d8c0ad25ec1745c20424f1dd3ff2c99ab59e97cf91",
        strip_prefix = "curl-7.73.0",
        urls = ["https://curl.haxx.se/download/curl-7.73.0.tar.gz"],
    )
