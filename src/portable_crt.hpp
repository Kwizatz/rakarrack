// portable_crt.hpp — Cross-platform wrappers for MSVC-deprecated CRT functions.
//
// Provides safe, warning-free replacements for fopen, getenv, strtok, and
// sscanf on both MSVC (_s variants) and POSIX (standard / _r variants).
// For sprintf → use snprintf(buf, sizeof(buf), ...) directly.
// For strcpy  → use snprintf(dst, sizeof(dst), "%s", src) directly.

#ifndef PORTABLE_CRT_HPP
#define PORTABLE_CRT_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

namespace rkr {

// ---- fopen ----

inline FILE* portable_fopen(const char* filename, const char* mode)
{
#ifdef _MSC_VER
    FILE* fp = nullptr;
    if (fopen_s(&fp, filename, mode) != 0)
        fp = nullptr;
    return fp;
#else
    return std::fopen(filename, mode);
#endif
}

// ---- getenv (returns std::string; empty if variable is unset) ----

inline std::string portable_getenv(const char* name)
{
#ifdef _MSC_VER
    char* val = nullptr;
    std::size_t len = 0;
    if (_dupenv_s(&val, &len, name) == 0 && val != nullptr) {
        std::string result(val);
        std::free(val);
        return result;
    }
    return {};
#else
    const char* val = std::getenv(name);
    return val ? std::string(val) : std::string{};
#endif
}

// ---- strtok ----

inline char* portable_strtok(char* str, const char* delim, char** context)
{
#ifdef _MSC_VER
    return strtok_s(str, delim, context);
#else
    return strtok_r(str, delim, context);
#endif
}

// ---- sscanf (numeric-only format strings — no %s/%c/%[ ) ----
//
// For format strings that only use %d, %f, %lf, etc., sscanf_s on MSVC is
// ABI-compatible with sscanf.  This macro expands to the correct function.

#ifdef _MSC_VER
#define RKR_SSCANF sscanf_s
#else
#define RKR_SSCANF sscanf
#endif

} // namespace rkr

#endif // PORTABLE_CRT_HPP
