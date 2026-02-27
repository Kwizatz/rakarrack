// compat_time.hpp - Cross-platform gettimeofday shim.
#ifndef COMPAT_TIME_HPP
#define COMPAT_TIME_HPP

#ifdef _MSC_VER
#include <windows.h>

struct compat_timeval {
    long tv_sec;
    long tv_usec;
};

inline int gettimeofday(struct compat_timeval* tp, void*) {
    if (tp) {
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        ULARGE_INTEGER li;
        li.LowPart = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        // Convert from 100-ns intervals since 1601-01-01 to Unix epoch
        li.QuadPart -= 116444736000000000ULL;
        tp->tv_sec = static_cast<long>(li.QuadPart / 10000000ULL);
        tp->tv_usec = static_cast<long>((li.QuadPart % 10000000ULL) / 10);
    }
    return 0;
}

#else
#include <sys/time.h>
using compat_timeval = struct timeval;
#endif

#endif // COMPAT_TIME_HPP
