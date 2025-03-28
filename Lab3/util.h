#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

#define FMT_CLEAR "\033[0m"
#define FMT_BOLD "\033[1m"
// debug logging only when running `make debug` :-)
#ifdef DEBUGGING
#define debug(...) printf(FMT_BOLD "[DEBUG]" FMT_CLEAR " " __VA_ARGS__)
#else
#define debug(...) // no-op
#endif

#endif // UTIL_H
