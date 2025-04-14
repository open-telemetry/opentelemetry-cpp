// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <optional>
#include <string>
#include <cstddef>

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace ext
{
namespace grpc
{
    #define OPENTELEMETRY_EXT_GRPC_OVERRIDES_X_ENUM(_) \
        _(int32_t, client_channel_backup_poll_interval_ms, ClientChannelBackupPollIntervalMs) \
        _(bool, enable_fork_support, EnableForkSupport) \
        _(bool, abort_on_leaks, AbortOnLeaks) \
        _(bool, not_use_system_ssl_roots, NotUseSystemSslRoots) \
        _(bool, cpp_experimental_disable_reflection, CppExperimentalDisableReflection) \
        _(std::string, dns_resolver, DnsResolver) \
        _(std::string, verbosity, Verbosity) \
        _(std::string, poll_strategy, PollStrategy) \
        _(std::string, system_ssl_roots_dir, SystemSslRootsDir) \
        _(std::string, default_ssl_roots_file_path, DefaultSslRootsFilePath) \
        _(std::string, ssl_cipher_suites, SslCipherSuites) \
        _(std::string, experiments, Experiments) \
        _(std::string, trace, Trace)

    struct Overrides {
        #define OPENTELEMETRY_EXT_GRPC_OVERRIDES_X_DEF(type,name,_) std::optional<type> name;
        OPENTELEMETRY_EXT_GRPC_OVERRIDES_X_ENUM(OPENTELEMETRY_EXT_GRPC_OVERRIDES_X_DEF)
        #undef OPENTELEMETRY_EXT_GRPC_OVERRIDES_X_DEF
    };

    OPENTELEMETRY_EXPORT std::string OverridesToString();
    OPENTELEMETRY_EXPORT void ResetOverrides();
    OPENTELEMETRY_EXPORT void SetOverrides(const Overrides& o);
    OPENTELEMETRY_EXPORT Overrides GetOverrides();
}  // namespace grpc
}  // namespace ext
OPENTELEMETRY_END_NAMESPACE