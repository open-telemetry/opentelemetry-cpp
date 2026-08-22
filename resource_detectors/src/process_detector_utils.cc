// Copyright The OpenTelemetry Authors
// SPDX-License-Identifier: Apache-2.0

#include "opentelemetry/resource_detectors/detail/process_detector_utils.h"

#include <cstdio>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>

#ifdef _MSC_VER
// clang-format off
#  include <windows.h>
#  include <psapi.h>
#  include <shellapi.h>
#  pragma comment(lib, "shell32.lib")
// clang-format on
#else
#  include <sys/types.h>
#  include <unistd.h>
#endif

#ifdef __APPLE__
#  include <sys/sysctl.h>
#endif

#ifndef _MSC_VER
#  include <pwd.h>
#endif

#include "opentelemetry/version.h"

OPENTELEMETRY_BEGIN_NAMESPACE
namespace resource_detector
{
namespace detail
{

constexpr const char *kExecutableName = "exe";
constexpr const char *kCmdlineName    = "cmdline";

ExecutableInfo GetExecutableInfo(const int32_t &pid)
{
  ExecutableInfo info;
#ifdef _MSC_VER
  HANDLE hProcess =
      OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, static_cast<DWORD>(pid));
  if (!hProcess)
  {
    return info;
  }

  WCHAR wbuffer[MAX_PATH];
  DWORD len    = MAX_PATH;
  BOOL success = QueryFullProcessImageNameW(hProcess, 0, wbuffer, &len);
  CloseHandle(hProcess);

  if (!success || len == 0)
  {
    return info;
  }

  // Convert UTF-16 to UTF-8
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, wbuffer, len, NULL, 0, NULL, NULL);
  if (size_needed <= 0)
  {
    return info;
  }
  std::string utf8_path(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, wbuffer, len, &utf8_path[0], size_needed, NULL, NULL);

  info.path = utf8_path;
#else
  std::string proc_path = FormFilePath(pid, kExecutableName);
  char buffer[4096];

  ssize_t len = readlink(proc_path.c_str(), buffer, sizeof(buffer) - 1);
  if (len != -1)
  {
    buffer[len] = '\0';
    info.path   = std::string(buffer);
  }
#endif

  if (!info.path.empty())
  {
    std::size_t sep = info.path.find_last_of("/\\");
    if (sep != std::string::npos)
    {
      info.name = info.path.substr(sep + 1);
    }
    else
    {
      info.name = info.path;
    }
  }
  return info;
}

std::vector<std::string> ExtractCommandWithArgs(const std::string &command_line_path)
{
  std::vector<std::string> commands;
  std::ifstream command_line_file(command_line_path, std::ios::in | std::ios::binary);
  std::string command;
  while (std::getline(command_line_file, command, '\0'))
  {
    if (!command.empty())
    {
      commands.push_back(command);
    }
  }
  return commands;
}

std::vector<std::string> GetCommandWithArgs(const int32_t &pid)
{
#ifdef _MSC_VER
  int argc      = 0;
  LPWSTR *argvW = CommandLineToArgvW(GetCommandLineW(), &argc);
  if (!argvW)
  {
    return {};  // returns an empty vector if CommandLineToArgvW fails
  }

  std::vector<std::string> args;
  for (int i = 0; i < argc; i++)
  {
    // Convert UTF-16 to UTF-8
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, NULL, 0, NULL, NULL);
    if (size_needed > 0)
    {
      std::string arg(size_needed - 1, 0);
      WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, &arg[0], size_needed, NULL, NULL);
      args.push_back(arg);
    }
  }

  LocalFree(argvW);
  return args;
#else
  std::string command_line_path = FormFilePath(pid, kCmdlineName);
  return ExtractCommandWithArgs(command_line_path);
#endif
}

std::string FormFilePath(const int32_t &pid, const char *process_type)
{
  char buff[64];
  int len = std::snprintf(buff, sizeof(buff), "/proc/%d/%s", pid, process_type);
  if (len < 0)
  {
    // in case snprintf fails
    return std::string();
  }
  if (len >= static_cast<int>(sizeof(buff)))
  {
    return std::string(buff, sizeof(buff) - 1);
  }
  return std::string(buff, len);
}

// ---------------------------------------------------------------------------
// GetProcessCreationTime
// ---------------------------------------------------------------------------

#ifdef _MSC_VER
namespace
{
// Convert a FILETIME to an ISO 8601 UTC string "YYYY-MM-DDTHH:MM:SS.mmmZ".
std::string FileTimeToIso8601(const FILETIME &ft)
{
  SYSTEMTIME st;
  if (!FileTimeToSystemTime(&ft, &st))
  {
    return std::string();
  }
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ", static_cast<int>(st.wYear),
                static_cast<int>(st.wMonth), static_cast<int>(st.wDay), static_cast<int>(st.wHour),
                static_cast<int>(st.wMinute), static_cast<int>(st.wSecond),
                static_cast<int>(st.wMilliseconds));
  return std::string(buf);
}
}  // namespace
#endif  // _MSC_VER

#if !defined(_MSC_VER) && !defined(__APPLE__)
namespace
{
// Parse the 22nd field (starttime, in clock ticks since boot) from /proc/<pid>/stat.
// The comm field (2nd) may contain spaces/parens, so we scan past the closing ')'.
bool ParseStarttimeFromProcStat(const std::string &stat_path, uint64_t &starttime_ticks)
{
  std::ifstream f(stat_path);
  if (!f.is_open())
  {
    return false;
  }
  std::string line;
  if (!std::getline(f, line))
  {
    return false;
  }
  // Skip past the closing ')' of the comm field.
  std::size_t pos = line.rfind(')');
  if (pos == std::string::npos)
  {
    return false;
  }
  pos += 2;  // skip ') '
  // Fields 3..21 (19 fields) come before starttime (field 22, 0-indexed from 3 → index 19).
  for (int i = 0; i < 19; ++i)
  {
    pos = line.find(' ', pos);
    if (pos == std::string::npos)
    {
      return false;
    }
    ++pos;
  }
  starttime_ticks = std::stoull(line.substr(pos));
  return true;
}

// Read boot time in seconds since epoch from /proc/stat.
bool ReadBootTimeSecs(uint64_t &boot_time)
{
  std::ifstream f("/proc/stat");
  if (!f.is_open())
  {
    return false;
  }
  std::string line;
  while (std::getline(f, line))
  {
    if (line.substr(0, 6) == "btime ")
    {
      boot_time = std::stoull(line.substr(6));
      return true;
    }
  }
  return false;
}
}  // namespace
#endif  // !_MSC_VER && !__APPLE__

std::string GetProcessCreationTime(const int32_t &pid)
{
#ifdef _MSC_VER
  HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, static_cast<DWORD>(pid));
  if (!hProcess)
  {
    return std::string();
  }
  FILETIME creation_time, exit_time, kernel_time, user_time;
  bool ok = GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time) != 0;
  CloseHandle(hProcess);
  if (!ok)
  {
    return std::string();
  }
  return FileTimeToIso8601(creation_time);

#elif defined(__APPLE__)
  struct kinfo_proc kp;
  std::size_t len = sizeof(kp);
  int mib[4]      = {CTL_KERN, KERN_PROC, KERN_PROC_PID, static_cast<int>(pid)};
  if (sysctl(mib, 4, &kp, &len, nullptr, 0) != 0 || len == 0)
  {
    return std::string();
  }
  // kp_proc.p_starttime is a struct timeval (seconds + microseconds).
  time_t secs        = kp.kp_proc.p_starttime.tv_sec;
  int64_t usecs      = kp.kp_proc.p_starttime.tv_usec;
  struct tm utc_time = {};
  gmtime_r(&secs, &utc_time);
  char buf[32];
  std::snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d.%03ldZ", utc_time.tm_year + 1900,
                utc_time.tm_mon + 1, utc_time.tm_mday, utc_time.tm_hour, utc_time.tm_min,
                utc_time.tm_sec, usecs / 1000);
  return std::string(buf);

#else
  // Linux: starttime (ticks since boot) from /proc/<pid>/stat + btime from /proc/stat.
  uint64_t starttime_ticks = 0;
  uint64_t boot_time_secs  = 0;

  std::string stat_path = FormFilePath(pid, "stat");
  if (!ParseStarttimeFromProcStat(stat_path, starttime_ticks))
  {
    return std::string();
  }
  if (!ReadBootTimeSecs(boot_time_secs))
  {
    return std::string();
  }

  int64_t clk_tck = sysconf(_SC_CLK_TCK);
  if (clk_tck <= 0)
  {
    return std::string();
  }

  const auto clk       = static_cast<uint64_t>(clk_tck);
  uint64_t start_secs  = boot_time_secs + starttime_ticks / clk;
  uint64_t start_msecs = (starttime_ticks % clk) * 1000 / clk;

  time_t t           = static_cast<time_t>(start_secs);
  struct tm utc_time = {};
  gmtime_r(&t, &utc_time);

  char buf[32];
  std::snprintf(buf, sizeof(buf), "%04d-%02d-%02dT%02d:%02d:%02d.%03lluZ", utc_time.tm_year + 1900,
                utc_time.tm_mon + 1, utc_time.tm_mday, utc_time.tm_hour, utc_time.tm_min,
                utc_time.tm_sec, static_cast<unsigned long long>(start_msecs));
  return std::string(buf);
#endif
}

// ---------------------------------------------------------------------------
// GetProcessOwner
// ---------------------------------------------------------------------------

std::string GetProcessOwner()
{
#ifdef _MSC_VER
  // On Windows, open the current process token and look up the account SID.
  HANDLE hToken = nullptr;
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
  {
    return std::string();
  }

  DWORD token_info_len = 0;
  GetTokenInformation(hToken, TokenUser, nullptr, 0, &token_info_len);
  if (token_info_len == 0)
  {
    CloseHandle(hToken);
    return std::string();
  }

  std::vector<BYTE> token_info_buf(token_info_len);
  if (!GetTokenInformation(hToken, TokenUser, token_info_buf.data(), token_info_len,
                           &token_info_len))
  {
    CloseHandle(hToken);
    return std::string();
  }
  CloseHandle(hToken);

  TOKEN_USER *token_user = reinterpret_cast<TOKEN_USER *>(token_info_buf.data());
  WCHAR name[256];
  WCHAR domain[256];
  DWORD name_len   = 256;
  DWORD domain_len = 256;
  SID_NAME_USE sid_use;
  if (!LookupAccountSidW(nullptr, token_user->User.Sid, name, &name_len, domain, &domain_len,
                         &sid_use))
  {
    return std::string();
  }

  // Convert UTF-16 username to UTF-8.
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, name, -1, nullptr, 0, nullptr, nullptr);
  if (size_needed <= 0)
  {
    return std::string();
  }
  std::string utf8_name(size_needed - 1, '\0');
  WideCharToMultiByte(CP_UTF8, 0, name, -1, &utf8_name[0], size_needed, nullptr, nullptr);
  return utf8_name;

#else
  // POSIX (Linux + macOS): resolve effective UID to a username via getpwuid_r.
  uid_t uid             = getuid();
  struct passwd pw      = {};
  struct passwd *result = nullptr;
  char buf[1024];
  if (getpwuid_r(uid, &pw, buf, sizeof(buf), &result) != 0 || result == nullptr)
  {
    return std::string();
  }
  return std::string(result->pw_name);
#endif
}

// ---------------------------------------------------------------------------
// GetExecutableBuildIdHtlhash  (self-contained SHA-256, no external deps)
// ---------------------------------------------------------------------------

namespace
{

// Minimal self-contained SHA-256 implementation.
// Reference: FIPS 180-4
struct Sha256Context
{
  uint32_t state[8]   = {0};
  uint64_t bit_count  = 0;
  uint8_t buffer[64]  = {0};
  uint32_t buffer_len = 0;
};

static const uint32_t kSha256K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

inline uint32_t Rotr32(uint32_t x, int n)
{
  return (x >> n) | (x << (32 - n));
}

void Sha256ProcessBlock(Sha256Context &ctx, const uint8_t block[64])
{
  uint32_t w[64];
  for (int i = 0; i < 16; ++i)
  {
    w[i] = (static_cast<uint32_t>(block[i * 4]) << 24) |
           (static_cast<uint32_t>(block[i * 4 + 1]) << 16) |
           (static_cast<uint32_t>(block[i * 4 + 2]) << 8) |
           (static_cast<uint32_t>(block[i * 4 + 3]));
  }
  for (int i = 16; i < 64; ++i)
  {
    uint32_t s0 = Rotr32(w[i - 15], 7) ^ Rotr32(w[i - 15], 18) ^ (w[i - 15] >> 3);
    uint32_t s1 = Rotr32(w[i - 2], 17) ^ Rotr32(w[i - 2], 19) ^ (w[i - 2] >> 10);
    w[i]        = w[i - 16] + s0 + w[i - 7] + s1;
  }

  uint32_t a = ctx.state[0], b = ctx.state[1], c = ctx.state[2], d = ctx.state[3];
  uint32_t e = ctx.state[4], f = ctx.state[5], g = ctx.state[6], h = ctx.state[7];

  for (int i = 0; i < 64; ++i)
  {
    uint32_t S1    = Rotr32(e, 6) ^ Rotr32(e, 11) ^ Rotr32(e, 25);
    uint32_t ch    = (e & f) ^ (~e & g);
    uint32_t temp1 = h + S1 + ch + kSha256K[i] + w[i];
    uint32_t S0    = Rotr32(a, 2) ^ Rotr32(a, 13) ^ Rotr32(a, 22);
    uint32_t maj   = (a & b) ^ (a & c) ^ (b & c);
    uint32_t temp2 = S0 + maj;

    h = g;
    g = f;
    f = e;
    e = d + temp1;
    d = c;
    c = b;
    b = a;
    a = temp1 + temp2;
  }

  ctx.state[0] += a;
  ctx.state[1] += b;
  ctx.state[2] += c;
  ctx.state[3] += d;
  ctx.state[4] += e;
  ctx.state[5] += f;
  ctx.state[6] += g;
  ctx.state[7] += h;
}

void Sha256Init(Sha256Context &ctx)
{
  ctx.state[0]   = 0x6a09e667;
  ctx.state[1]   = 0xbb67ae85;
  ctx.state[2]   = 0x3c6ef372;
  ctx.state[3]   = 0xa54ff53a;
  ctx.state[4]   = 0x510e527f;
  ctx.state[5]   = 0x9b05688c;
  ctx.state[6]   = 0x1f83d9ab;
  ctx.state[7]   = 0x5be0cd19;
  ctx.bit_count  = 0;
  ctx.buffer_len = 0;
}

void Sha256Update(Sha256Context &ctx, const uint8_t *data, std::size_t len)
{
  for (std::size_t i = 0; i < len; ++i)
  {
    ctx.buffer[ctx.buffer_len++] = data[i];
    if (ctx.buffer_len == 64)
    {
      Sha256ProcessBlock(ctx, ctx.buffer);
      ctx.buffer_len = 0;
    }
  }
  ctx.bit_count += static_cast<uint64_t>(len) * 8;
}

void Sha256Final(Sha256Context &ctx, uint8_t digest[32])
{
  ctx.buffer[ctx.buffer_len++] = 0x80;
  if (ctx.buffer_len > 56)
  {
    while (ctx.buffer_len < 64)
    {
      ctx.buffer[ctx.buffer_len++] = 0x00;
    }
    Sha256ProcessBlock(ctx, ctx.buffer);
    ctx.buffer_len = 0;
  }
  while (ctx.buffer_len < 56)
  {
    ctx.buffer[ctx.buffer_len++] = 0x00;
  }
  // Append bit count as big-endian 64-bit integer.
  for (int i = 7; i >= 0; --i)
  {
    ctx.buffer[ctx.buffer_len++] = static_cast<uint8_t>((ctx.bit_count >> (i * 8)) & 0xFF);
  }
  Sha256ProcessBlock(ctx, ctx.buffer);

  for (int i = 0; i < 8; ++i)
  {
    digest[i * 4]     = static_cast<uint8_t>((ctx.state[i] >> 24) & 0xFF);
    digest[i * 4 + 1] = static_cast<uint8_t>((ctx.state[i] >> 16) & 0xFF);
    digest[i * 4 + 2] = static_cast<uint8_t>((ctx.state[i] >> 8) & 0xFF);
    digest[i * 4 + 3] = static_cast<uint8_t>(ctx.state[i] & 0xFF);
  }
}

// Encode the first `byte_count` bytes of `digest` as lowercase hex.
std::string DigestToHex(const uint8_t *digest, std::size_t byte_count)
{
  static const char kHex[] = "0123456789abcdef";
  std::string result;
  result.reserve(byte_count * 2);
  for (std::size_t i = 0; i < byte_count; ++i)
  {
    result += kHex[(digest[i] >> 4) & 0x0F];
    result += kHex[digest[i] & 0x0F];
  }
  return result;
}

}  // namespace

std::string GetExecutableBuildIdHtlhash(const int32_t &pid)
{
  std::string exe_path = GetExecutableInfo(pid).path;
  if (exe_path.empty())
  {
    return std::string();
  }

  std::ifstream f(exe_path, std::ios::binary | std::ios::ate);
  if (!f.is_open())
  {
    return std::string();
  }

  const auto end_pos = f.tellg();
  if (end_pos < 0)
  {
    return std::string();
  }
  auto file_size = static_cast<std::uint64_t>(end_pos);

  constexpr std::size_t kChunkSize = 4096;

  // Read head (up to 4096 bytes).
  std::string head(kChunkSize, '\0');
  f.seekg(0, std::ios::beg);
  f.read(&head[0], static_cast<std::streamsize>(kChunkSize));
  std::size_t head_read = static_cast<std::size_t>(f.gcount());
  head.resize(head_read);

  // Read tail (up to 4096 bytes from end) only if file is larger than 4096 bytes.
  std::string tail;
  if (file_size > kChunkSize)
  {
    tail.resize(kChunkSize, '\0');
    std::size_t tail_offset = static_cast<std::size_t>(file_size - kChunkSize);
    f.seekg(static_cast<std::streamoff>(tail_offset), std::ios::beg);
    f.read(&tail[0], static_cast<std::streamsize>(kChunkSize));
    std::size_t tail_read = static_cast<std::size_t>(f.gcount());
    tail.resize(tail_read);
  }

  // Encode file length as big-endian uint64.
  std::uint64_t file_size_be = file_size;
  uint8_t len_bytes[8];
  for (int i = 7; i >= 0; --i)
  {
    len_bytes[i] = static_cast<uint8_t>(file_size_be & 0xFF);
    file_size_be >>= 8;
  }

  // SHA256(head || tail || len_bytes).
  Sha256Context ctx;
  Sha256Init(ctx);
  Sha256Update(ctx, reinterpret_cast<const uint8_t *>(head.data()), head.size());
  Sha256Update(ctx, reinterpret_cast<const uint8_t *>(tail.data()), tail.size());
  Sha256Update(ctx, len_bytes, 8);

  uint8_t digest[32];
  Sha256Final(ctx, digest);

  // Return first 16 bytes (128 bits) as hex (32 hex chars).
  return DigestToHex(digest, 16);
}

std::string ComputeSha256Hex(const std::string &data)
{
  Sha256Context ctx;
  Sha256Init(ctx);
  Sha256Update(ctx, reinterpret_cast<const uint8_t *>(data.data()), data.size());

  uint8_t digest[32];
  Sha256Final(ctx, digest);

  // Return all 32 bytes (256 bits) as hex (64 hex chars).
  return DigestToHex(digest, 32);
}

}  // namespace detail
}  // namespace resource_detector
OPENTELEMETRY_END_NAMESPACE
