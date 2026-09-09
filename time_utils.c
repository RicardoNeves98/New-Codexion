#include "codexion.h"

long get_time_diff(struct timespec time1, struct timespec time2)
{
    return (timespec_to_ms(time1) - timespec_to_ms(time2)); 
}

struct timespec get_min_time(struct timespec time1, struct timespec time2)
{
    if (get_time_diff(time1, time2) < 0)
        return (time1);
    return (time2);
}

struct timespec add_time(struct timespec time1, struct timespec time2)
{
    struct timespec result;
    long total_nano;

    total_nano = time1.tv_nsec + time2.tv_nsec;
    result.tv_nsec = total_nano % 1000000000;
    result.tv_sec = time1.tv_sec + time2.tv_sec + (total_nano / 1000000000);
    return (result);
}

struct timespec add_curr_time(struct timespec time)
{
    struct timespec now;

    clock_gettime(CLOCK_REALTIME, &now);
    return (add_time(now, time));
}


