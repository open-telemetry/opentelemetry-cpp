# Copyright The OpenTelemetry Authors
# SPDX-License-Identifier: Apache-2.0

# Compiler options for building libcurl.

BASE_CURL_COPTS = [
    # Disable everything else except HTTP protocol.
    "-DHTTP_ONLY=1",
    "-DENABLE_IPV6=1",
    "-DGETHOSTNAME_TYPE_ARG2=size_t",
    "-DGETSERVBYPORT_R_ARGS=6",
    "-DGETSERVBYPORT_R_BUFSIZE=4096",
    "-DHAVE_ALARM=1",
    "-DHAVE_ALLOCA_H=1",
    "-DHAVE_ARPA_INET_H=1",
    "-DHAVE_ARPA_TFTP_H=1",
    "-DHAVE_ASSERT_H=1",
    "-DHAVE_BASENAME=1",
    "-DHAVE_BOOL_T=1",
    "-DHAVE_CLOCK_GETTIME_MONOTONIC=1",
    "-DHAVE_CONNECT=1",
    "-DHAVE_DLFCN_H=1",
    "-DHAVE_ENGINE_LOAD_BUILTIN_ENGINES=1",
    "-DHAVE_ERRNO_H=1",
    "-DHAVE_FCNTL=1",
    "-DHAVE_FCNTL_H=1",
    "-DHAVE_FCNTL_O_NONBLOCK=1",
    "-DHAVE_FDOPEN=1",
    "-DHAVE_FREEADDRINFO=1",
    "-DHAVE_FREEIFADDRS=1",
    "-DHAVE_FSETXATTR=1",
    "-DHAVE_FSETXATTR_5=1",
    "-DHAVE_FTRUNCATE=1",
    "-DHAVE_GAI_STRERROR=1",
    "-DHAVE_GETADDRINFO=1",
    "-DHAVE_GETADDRINFO_THREADSAFE=1",
    "-DHAVE_GETEUID=1",
    "-DHAVE_GETHOSTBYADDR=1",
    "-DHAVE_GETHOSTBYADDR_R=1",
    "-DHAVE_GETHOSTBYADDR_R_8=1",
    "-DHAVE_GETHOSTBYNAME=1",
    "-DHAVE_GETHOSTBYNAME_R=1",
    "-DHAVE_GETHOSTBYNAME_R_6=1",
    "-DHAVE_GETHOSTNAME=1",
    "-DHAVE_GETIFADDRS=1",
    "-DHAVE_GETNAMEINFO=1",
    "-DHAVE_GETPPID=1",
    "-DHAVE_GETPWUID=1",
    "-DHAVE_GETPWUID_R=1",
    "-DHAVE_GETRLIMIT=1",
    "-DHAVE_GETSERVBYPORT_R=1",
    "-DHAVE_GETTIMEOFDAY=1",
    "-DHAVE_GMTIME_R=1",
    "-DHAVE_IFADDRS_H=1",
    "-DHAVE_IF_NAMETOINDEX=1",
    "-DHAVE_INET_NTOP=1",
    "-DHAVE_INET_PTON=1",
    "-DHAVE_INTTYPES_H=1",
    "-DHAVE_IOCTL=1",
    "-DHAVE_IOCTL_FIONBIO=1",
    "-DHAVE_IOCTL_SIOCGIFADDR=1",
    "-DHAVE_LIBGEN_H=1",
    "-DHAVE_LL=1",
    "-DHAVE_LOCALE_H=1",
    "-DHAVE_LOCALTIME_R=1",
    "-DHAVE_LONGLONG=1",
    "-DHAVE_MALLOC_H=1",
    "-DHAVE_MEMORY_H=1",
    "-DHAVE_NETDB_H=1",
    "-DHAVE_NETINET_IN_H=1",
    "-DHAVE_NETINET_TCP_H=1",
    "-DHAVE_NET_IF_H=1",
    "-DHAVE_PIPE=1",
    "-DHAVE_POLL=1",
    "-DHAVE_POLL_FINE=1",
    "-DHAVE_POLL_H=1",
    "-DHAVE_POSIX_STRERROR_R=1",
    "-DHAVE_PTHREAD_H=1",
    "-DHAVE_PWD_H=1",
    "-DHAVE_RECV=1",
    "-DHAVE_SELECT=1",
    "-DHAVE_SEND=1",
    "-DHAVE_SETJMP_H=1",
    "-DHAVE_SETLOCALE=1",
    "-DHAVE_SETRLIMIT=1",
    "-DHAVE_SETSOCKOPT=1",
    "-DHAVE_SGTTY_H=1",
    "-DHAVE_SIGACTION=1",
    "-DHAVE_SIGINTERRUPT=1",
    "-DHAVE_SIGNAL=1",
    "-DHAVE_SIGNAL_H=1",
    "-DHAVE_SIGSETJMP=1",
    "-DHAVE_SIG_ATOMIC_T=1",
    "-DHAVE_SOCKADDR_IN6_SIN6_SCOPE_ID=1",
    "-DHAVE_SOCKET=1",
    "-DHAVE_SOCKETPAIR=1",
    "-DHAVE_STDBOOL_H=1",
    "-DHAVE_STDINT_H=1",
    "-DHAVE_STDIO_H=1",
    "-DHAVE_STDLIB_H=1",
    "-DHAVE_STRCASECMP=1",
    "-DHAVE_STRDUP=1",
    "-DHAVE_STRERROR_R=1",
    "-DHAVE_STRINGS_H=1",
    "-DHAVE_STRING_H=1",
    "-DHAVE_STRNCASECMP=1",
    "-DHAVE_STRSTR=1",
    "-DHAVE_STRTOK_R=1",
    "-DHAVE_STRTOLL=1",
    "-DHAVE_STRUCT_SOCKADDR_STORAGE=1",
    "-DHAVE_STRUCT_TIMEVAL=1",
    "-DHAVE_SYS_IOCTL_H=1",
    "-DHAVE_SYS_PARAM_H=1",
    "-DHAVE_SYS_POLL_H=1",
    "-DHAVE_SYS_RESOURCE_H=1",
    "-DHAVE_SYS_SELECT_H=1",
    "-DHAVE_SYS_SOCKET_H=1",
    "-DHAVE_SYS_STAT_H=1",
    "-DHAVE_SYS_TIME_H=1",
    "-DHAVE_SYS_TYPES_H=1",
    "-DHAVE_SYS_UIO_H=1",
    "-DHAVE_SYS_UN_H=1",
    "-DHAVE_SYS_WAIT_H=1",
    "-DHAVE_SYS_XATTR_H=1",
    "-DHAVE_TERMIOS_H=1",
    "-DHAVE_TERMIO_H=1",
    "-DHAVE_TIME_H=1",
    "-DHAVE_UNISTD_H=1",
    "-DHAVE_UTIME=1",
    "-DHAVE_UTIMES=1",
    "-DHAVE_UTIME_H=1",
    "-DHAVE_VARIADIC_MACROS_C99=1",
    "-DHAVE_VARIADIC_MACROS_GCC=1",
    "-DHAVE_WRITABLE_ARGV=1",
    "-DHAVE_WRITEV=1",
    "-DRECV_TYPE_ARG1=int",
    "-DRECV_TYPE_ARG2=void*",
    "-DRECV_TYPE_ARG3=size_t",
    "-DRECV_TYPE_ARG4=int",
    "-DRECV_TYPE_RETV=ssize_t",
    "-DRETSIGTYPE=void",
    "-DSELECT_QUAL_ARG5=",
    "-DSELECT_TYPE_ARG1=int",
    "-DSELECT_TYPE_ARG234=fd_set*",
    "-DSELECT_TYPE_RETV=int",
    "-DSEND_QUAL_ARG2=const",
    "-DSEND_TYPE_ARG1=int",
    "-DSEND_TYPE_ARG2=void*",
    "-DSEND_TYPE_ARG3=size_t",
    "-DSEND_TYPE_ARG4=int",
    "-DSEND_TYPE_RETV=ssize_t",
    "-DSIZEOF_CURL_OFF_T=8",
    "-DSIZEOF_INT=4",
    "-DSIZEOF_LONG=8",
    "-DSIZEOF_OFF_T=8",
    "-DSIZEOF_SHORT=2",
    "-DSIZEOF_SIZE_T=8",
    "-DSIZEOF_TIME_T=8",
    "-DSTDC_HEADERS=1",
    "-DSTRERROR_R_TYPE_ARG3=size_t",
    "-DTIME_WITH_SYS_TIME=1",
    "-DUSE_THREADS_POSIX=1",
    "-DUSE_UNIX_SOCKETS=1",

    # Extra defines needed by curl
    "-DBUILDING_LIBCURL",
    "-DCURL_HIDDEN_SYMBOLS",
]

LINUX_CURL_COPTS = [
    "-DHAVE_LINUX_TCP_H=1",
    "-DHAVE_MSG_NOSIGNAL=1",
]

CURL_COPTS = select({
    "//:windows": [
        # Disable everything else except HTTP protocol.
        "/DHTTP_ONLY=1",
        "/DCURL_STATICLIB",
        "/DWIN32",
        "/DBUILDING_LIBCURL",
        "/DUSE_WIN32_IDN",
        "/DWANT_IDN_PROTOTYPES",
        "/DUSE_IPV6",
        "/DUSE_WINDOWS_SSPI",
        "/DUSE_SCHANNEL",
    ],
    "//:osx": BASE_CURL_COPTS,
    "//conditions:default": BASE_CURL_COPTS + LINUX_CURL_COPTS,
})
