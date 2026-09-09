#include "codexion.h"

void *finish(struct monitor_state *monitor_info, int error)
{
    long time_passed;
    struct timespec now;

    pthread_mutex_lock(monitor_info->output_mutex);
    clock_gettime(CLOCK_MONOTONIC, &now);
    time_passed = get_time_diff(now, monitor_info->start_time);
    if (error || *monitor_info->coders_active)
    {
        if (error)
            printf("%ld Error in pthread_cond_{wait, timedwait}", time_passed);
        else
            printf("%ld %d burned out\n", time_passed, monitor_info->deadline->id);
        *monitor_info->coders_active = 0;
    }
    else if (*monitor_info->coders_active == 0)
        printf("%ld Coders completed the work successfully!\n", time_passed);
    pthread_mutex_unlock(monitor_info->output_mutex);
    pthread_mutex_unlock(&monitor_info->queue->mutex);
    return (NULL);
}

void *monitor_func(void *info)
{
    int value;
    int coder_num;
    struct timespec time;
    struct monitor_state *monitor_info;

    monitor_info = (struct monitor_state *)info;
    pthread_mutex_lock(&monitor_info->queue->mutex);
    coder_num = *monitor_info->coders_active;
    while (monitor_info->deadline->id == 0)
    {
        value = pthread_cond_wait(&monitor_info->queue->cond,
                                  &monitor_info->queue->mutex);
        if (value != 0)
            return (finish(monitor_info, 1));
    }
    while (*monitor_info->coders_active > 0)
    {
        time = monitor_info->deadline->time;
        value = pthread_cond_timedwait(&monitor_info->queue->cond,
                                       &monitor_info->queue->mutex, &time);
        print_deadline(monitor_info->deadline, coder_num);
        if (value == ETIMEDOUT)
            return (finish(monitor_info, 0));
        else if (value != 0)
            return (finish(monitor_info, 1));      
    }
    return (finish(monitor_info, 0));
}
