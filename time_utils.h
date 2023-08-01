#ifndef UTILS_H_SENTRY
#define UTILS_H_SENTRY

#include <sys/time.h>

long milliseconds_between(const struct timeval *start,
                          const struct timeval *end);

#endif
