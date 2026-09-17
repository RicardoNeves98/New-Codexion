#include "codexion.h"

void *finish(struct monitor_state *monitor)
{
    long time_passed;
    struct timespec now;

    pthread_mutex_lock(monitor->output_mutex);
    if (clock_gettime(CLOCK_MONOTONIC, &now) == -1)
        *monitor->error = 1;
    if (*monitor->error == 0)
    {
        time_passed = get_time_diff(now, monitor->start_time);
        if (*monitor->coders_active > 0)
        {
            printf("%ld %d burned out\n", time_passed, monitor->deadline->id);
            *monitor->coders_active = 0;
        }
        else if (*monitor->coders_active == 0)
            printf("%ld Coders completed the work successfully!\n", time_passed);
    }
    else
        printf("Error happened on clock_gettime or pthread_cond_{wait, timedwait}\n");
    pthread_mutex_unlock(monitor->output_mutex);
    pthread_mutex_unlock(&monitor->queue->mutex);
    return (NULL);
}

int wait_deadline(struct thread_vars *queue, struct queue *deadline, int *error)
{
    int value;

    pthread_mutex_lock(&queue->mutex);
    while (deadline->id == 0 && *error == 0)
    {
        value = pthread_cond_wait(&queue->cond, &queue->mutex);
        if (value != 0 || *error == 1)
        {
            if (value != 0)
                *error = 1;
            pthread_mutex_unlock(&queue->mutex);
            return (0);
        }
    }
    pthread_mutex_unlock(&queue->mutex);
    return (1);
}

void *monitor_func(void *info)
{
    int value;
    struct timespec time;
    struct monitor_state *monitor;

    monitor = (struct monitor_state *)info;
    if (!check_all(monitor->queue, monitor->coders_active, monitor->error,
                   monitor->coder_num, monitor->init_limit))
        return (finish(monitor));
    if (!wait_deadline(monitor->queue, monitor->deadline, monitor->error))
        return (finish(monitor));
    pthread_mutex_lock(&monitor->queue->mutex);
    time = monitor->deadline->time;
    while (*monitor->coders_active > 0)
    {
        value = pthread_cond_timedwait(&monitor->queue->cond,
                                       &monitor->queue->mutex, &time);
        time = monitor->deadline->time;
        if (value == ETIMEDOUT)
            return (finish(monitor));
        else if (value != 0 || *monitor->error == 1)
            return (finish(monitor));
    }
    return (finish(monitor));
}

// I need to fix this problem 
// I could have a solution that was every time it went on timeout and all actually
// Have compiled than check the deadline id and if it is -1 just make him sleep more 
// That could he easily done by just adding something to the deadline time 
