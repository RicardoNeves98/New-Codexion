#include "codexion.h"

struct timespec ms_to_timespec(int time_ms)
{
    struct timespec new_format;

    new_format.tv_sec = time_ms / 1000;
    new_format.tv_nsec = (time_ms % 1000) * 1000000;
    return (new_format);
}

long timespec_to_ms(struct timespec time)
{
    long time_ms;

    time_ms = time.tv_sec * 1000;
    time_ms += time.tv_nsec / 1000000;
    return (time_ms);
}
