#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <locale.h>
#include <paths.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define UNUSED(x) (void)(x)

#define assert_or_die(x)   \
    do {                   \
        assert((x));       \
        if (!(x)) abort(); \
    } while (false)

size_t C_MB_CUR_MAX() { return MB_CUR_MAX; }

uint64_t C_ST_LOCAL() {
#if defined(ST_LOCAL)
    return ST_LOCAL;
#else
    return 0;
#endif
}

// confstr + _CS_PATH is only available on macOS with rust's libc
// we could just declare extern "C" confstr directly in Rust
// that would panic if it failed to link, which C++ did not
// therefore we define a backup, which just returns an error
// which for confstr is 0
#if defined(_CS_PATH)
#else
size_t confstr(int name, char* buf, size_t size) {
    UNUSED(name);
    UNUSED(buf);
    UNUSED(size);
    return 0;
}
#endif

int C_CS_PATH() {
#if defined(_CS_PATH)
    return _CS_PATH;
#else
    return -1;
#endif
}

uint64_t C_MNT_LOCAL() {
#if defined(MNT_LOCAL)
    return MNT_LOCAL;
#else
    return 0;
#endif
}

const char* C_PATH_BSHELL() { return _PATH_BSHELL; }

int C_PC_CASE_SENSITIVE() {
#ifdef _PC_CASE_SENSITIVE
    return _PC_CASE_SENSITIVE;
#else
    return 0;
#endif
}

FILE* stdout_stream() { return stdout; }

int C_RLIMIT_CORE() { return RLIMIT_CORE; }
int C_RLIMIT_DATA() { return RLIMIT_DATA; }
int C_RLIMIT_FSIZE() { return RLIMIT_FSIZE; }
int C_RLIMIT_NOFILE() { return RLIMIT_NOFILE; }
int C_RLIMIT_STACK() { return RLIMIT_STACK; }
int C_RLIMIT_CPU() { return RLIMIT_CPU; }

int C_RLIMIT_SBSIZE() {
#ifdef RLIMIT_SBSIZE
    return RLIMIT_SBSIZE;
#else
    return -1;
#endif
}

int C_RLIMIT_NICE() {
#ifdef RLIMIT_NICE
    return RLIMIT_NICE;
#else
    return -1;
#endif
}

int C_RLIMIT_SIGPENDING() {
#ifdef RLIMIT_SIGPENDING
    return RLIMIT_SIGPENDING;
#else
    return -1;
#endif
}

int C_RLIMIT_MEMLOCK() {
#ifdef RLIMIT_MEMLOCK
    return RLIMIT_MEMLOCK;
#else
    return -1;
#endif
}

int C_RLIMIT_RSS() {
#ifdef RLIMIT_RSS
    return RLIMIT_RSS;
#else
    return -1;
#endif
}

int C_RLIMIT_MSGQUEUE() {
#ifdef RLIMIT_MSGQUEUE
    return RLIMIT_MSGQUEUE;
#else
    return -1;
#endif
}

int C_RLIMIT_RTPRIO() {
#ifdef RLIMIT_RTPRIO
    return RLIMIT_RTPRIO;
#else
    return -1;
#endif
}

int C_RLIMIT_NPROC() {
#ifdef RLIMIT_NPROC
    return RLIMIT_NPROC;
#else
    return -1;
#endif
}

int C_RLIMIT_AS() {
#ifdef RLIMIT_AS
    return RLIMIT_AS;
#else
    return -1;
#endif
}

int C_RLIMIT_SWAP() {
#ifdef RLIMIT_SWAP
    return RLIMIT_SWAP;
#else
    return -1;
#endif
}

int C_RLIMIT_RTTIME() {
#ifdef RLIMIT_RTTIME
    return RLIMIT_RTTIME;
#else
    return -1;
#endif
}

int C_RLIMIT_KQUEUES() {
#ifdef RLIMIT_KQUEUES
    return RLIMIT_KQUEUES;
#else
    return -1;
#endif
}

int C_RLIMIT_NPTS() {
#ifdef RLIMIT_NPTS
    return RLIMIT_NPTS;
#else
    return -1;
#endif
}

int C_RLIMIT_NTHR() {
#ifdef RLIMIT_NTHR
    return RLIMIT_NTHR;
#else
    return -1;
#endif
}

bool C_readdir64(DIR* dirp, const char** d_name, size_t* d_name_len, uint64_t* d_ino,
                 unsigned char* d_type) {
    struct dirent* dent = readdir(dirp);
    if (!dent) {
        return false;
    }
    *d_name = dent->d_name;
    *d_name_len = sizeof dent->d_name / sizeof **d_name;
#if defined(__BSD__)
    *d_ino = dent->d_fileno;
#else
    *d_ino = dent->d_ino;
#endif
    *d_type = dent->d_type;
    return true;
}

bool C_fstatat64(int dirfd, const char* file, int flag, uint64_t* st_dev, uint64_t* st_ino,
                 mode_t* st_mode) {
    struct stat buf;
    if (fstatat(dirfd, file, &buf, flag) == -1) {
        return false;
    }
    *st_dev = buf.st_dev;
    *st_ino = buf.st_ino;
    *st_mode = buf.st_mode;
    return true;
}

bool C_localtime64_r(int64_t timep, struct tm* result) {
    time_t timep_ = timep;
    return localtime_r(&timep_, result);
}

struct timeval64 {
    int64_t tv_sec;
    int64_t tv_usec;
};

int C_select64(int nfds, fd_set* readfds, fd_set* writefds, fd_set* errorfds,
               struct timeval64* timeout64) {
    struct timeval timeout;
    if (timeout64) {
        timeout.tv_sec = timeout64->tv_sec;
        timeout.tv_usec = timeout64->tv_usec;
        assert_or_die(timeout.tv_sec == timeout64->tv_sec);
        assert_or_die(timeout.tv_usec == timeout64->tv_usec);
    }
    int result = select(nfds, readfds, writefds, errorfds, timeout64 ? &timeout : NULL);
    if (timeout64) {
        timeout64->tv_sec = timeout.tv_sec;
        timeout64->tv_usec = timeout.tv_usec;
    }
    return result;
}

struct timespec64 {
    int64_t tv_sec;
    int64_t tv_nsec;
};

int C_pselect64(int nfds, fd_set* readfds, fd_set* writefds, fd_set* errorfds,
                struct timespec64* timeout64, const sigset_t* sigmask) {
    struct timespec timeout;
    if (timeout64) {
        timeout.tv_sec = timeout64->tv_sec;
        timeout.tv_nsec = timeout64->tv_nsec;
        assert_or_die(timeout.tv_sec == timeout64->tv_sec);
        assert_or_die(timeout.tv_nsec == timeout64->tv_nsec);
    }
    return pselect(nfds, readfds, writefds, errorfds, timeout64 ? &timeout : NULL, sigmask);
}

struct rusage64 {
    struct timeval64 ru_utime;
    struct timeval64 ru_stime;
    int64_t ru_maxrss;
    int64_t ru_nsignals;
};

int C_getrusage64(int resource, struct rusage64* usage) {
    struct rusage tmp;
    int result = getrusage(resource, &tmp);
    usage->ru_utime.tv_sec = tmp.ru_utime.tv_sec;
    usage->ru_utime.tv_usec = tmp.ru_utime.tv_usec;
    usage->ru_stime.tv_sec = tmp.ru_stime.tv_sec;
    usage->ru_stime.tv_usec = tmp.ru_stime.tv_usec;
    usage->ru_maxrss = tmp.ru_maxrss;
    usage->ru_nsignals = tmp.ru_nsignals;
    return result;
}

bool C_clock_gettime64(clockid_t clock_id, struct timespec64* tp) {
    struct timespec tp_;
    if (clock_gettime(clock_id, &tp_) == -1) {
        return false;
    }
    tp->tv_sec = tp_.tv_sec;
    tp->tv_nsec = tp_.tv_nsec;
    return true;
}

bool C_futimens64(int fd, struct timespec64 times0, struct timespec64 times1) {
    struct timespec times[2];
    times[0].tv_sec = times0.tv_sec;
    times[0].tv_nsec = times0.tv_nsec;
    times[1].tv_sec = times1.tv_sec;
    times[1].tv_nsec = times1.tv_nsec;
    return futimens(fd, &times[0]) == 0;
}
