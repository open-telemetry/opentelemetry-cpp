load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def opentelemetry_cpp_deps():
    """Loads dependencies need to compile the opentelemetry-cpp library."""

    # Google Benchmark library.
    # Only needed for benchmarks, not to build the OpenTelemetry library.
    maybe(
        native.local_repository,
        name = "com_github_google_benchmark",
        path = "third_party/benchmark",
    )

    # GoogleTest framework.
    # Only needed for tests, not to build the OpenTelemetry library.
    maybe(
        native.local_repository,
        name = "com_google_googletest",
        path = "third_party/googletest",
    )

    # Load gRPC dependency
    maybe(
        http_archive,
        name = "com_github_grpc_grpc",
        sha256 = "d6277f77e0bb922d3f6f56c0f93292bb4cfabfc3c92b31ee5ccea0e100303612",
        strip_prefix = "grpc-1.28.0",
        urls = [
            "https://github.com/grpc/grpc/archive/v1.28.0.tar.gz",
        ],
    )

    # Uses older protobuf version because of
    # https://github.com/protocolbuffers/protobuf/issues/7179
    maybe(
        http_archive,
        name = "com_google_protobuf",
        sha256 = "b679cef31102ed8beddc39ecfd6368ee311cbee6f50742f13f21be7278781821",
        strip_prefix = "protobuf-3.11.2",
        urls = [
            "https://github.com/protocolbuffers/protobuf/releases/download/v3.11.2/protobuf-all-3.11.2.tar.gz",
        ],
    )

    # OTLP Protocol definition
    maybe(
        native.new_local_repository,
        name = "com_github_opentelemetry_proto",
        build_file = "//bazel:opentelemetry_proto.BUILD",
        path = "third_party/opentelemetry-proto",
    )

    # JSON library
    maybe(
        http_archive,
        name = "github_nlohmann_json",
        build_file = "//bazel:nlohmann_json.BUILD",
        sha256 = "69cc88207ce91347ea530b227ff0776db82dcb8de6704e1a3d74f4841bc651cf",
        urls = [
            "https://github.com/nlohmann/json/releases/download/v3.6.1/include.zip",
        ],
    )

    # C++ Prometheus Client library.
    maybe(
        native.local_repository,
        name = "com_github_jupp0r_prometheus_cpp",
        path = "third_party/prometheus-cpp",
    )

    # libcurl (optional)
    maybe(
        http_archive,
        name = "curl",
        build_file = "@//bazel:curl.BUILD",
        sha256 = "ba98332752257b47b9dea6d8c0ad25ec1745c20424f1dd3ff2c99ab59e97cf91",
        strip_prefix = "curl-7.73.0",
        urls = ["https://curl.haxx.se/download/curl-7.73.0.tar.gz"],
    )
