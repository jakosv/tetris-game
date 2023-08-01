#include "time_utils.h"

long milliseconds_between(const struct timeval *start,
                          const struct timeval *end)
{
    return (end->tv_sec - start->tv_sec) * 1000 +
        (end->tv_usec - start->tv_usec) / 1000;
}
