#include "codexion.h"

void *error_output(pthread_mutex_t *output_mutex)
{
    pthread_mutex_lock(output_mutex);
    printf("Error on clock_gettime or pthread_cond\n");
    pthread_mutex_unlock(output_mutex);
    return (NULL);
}

void final_output(pthread_mutex_t *output_mutex, struct timespec start,
                  int *coders_active, int burned_coder)
{
    long time_passed;
    struct timespec now;

    pthread_mutex_lock(output_mutex);
    if (burned_coder == -1 || clock_gettime(CLOCK_MONOTONIC, &now) == -1)
        printf("Error on clock_gettime or pthread_cond\n");
    else
    {
        time_passed = get_time_diff(now, start);
        if (burned_coder > 0)
        {
            printf("%ld %d burned out\n", time_passed, burned_coder);
            *coders_active = 0;
        }
        else
            printf("%ld Coders completed all the work!\n", time_passed);
    }
    pthread_mutex_unlock(output_mutex);
}

int wait_deadline(struct thread_vars *queue, struct queue *deadline, int *error)
{
    int value;

    pthread_mutex_lock(&queue->mutex);
    while (deadline->id == 0 && *error == 0)
    {
        value = pthread_cond_wait(&queue->cond, &queue->mutex);
        if (value != 0)
            *error = 1;
    }
    if (*error == 1)
    {
        pthread_mutex_unlock(&queue->mutex);
        return (0);
    }
    pthread_mutex_unlock(&queue->mutex);
    return (1);
}

int monitor_coders(struct queue *deadline, struct thread_vars *queue,
                   int *coders_active, int *error)
{
    int value;
    struct timespec time;

    time = deadline->time;
    while (*coders_active > 0 && *error == 0)
    {
        value = pthread_cond_timedwait(&queue->cond, &queue->mutex, &time);
        if (value == ETIMEDOUT && deadline->id == -1)
            time.tv_sec += 1;
        else if (value == ETIMEDOUT && deadline->id > 0)
            return (deadline->id);
        else if (value != 0)
            *error = 1;
        else
            time = deadline->time;
    }
    if (*error == 1)
        return (-1);
    return (0);
}

void *monitor_func(void *info)
{
    int burned_coder;
    int all_threads;
    struct monitor_state *monitor;

    monitor = (struct monitor_state *)info;
    all_threads = check_all(monitor->queue, monitor->coders_active, monitor->error,
                            monitor->coder_num, monitor->init_limit);
    if (all_threads == -1)
        return (error_output(monitor->output_mutex));
    else if (all_threads == 0)
        return (NULL);
    if (!wait_deadline(monitor->queue, monitor->deadline, monitor->error))
        return (error_output(monitor->output_mutex));
    pthread_mutex_lock(&monitor->queue->mutex);
    burned_coder = monitor_coders(monitor->deadline, monitor->queue,
                                  monitor->coders_active, monitor->error);
    final_output(monitor->output_mutex, monitor->start_time, monitor->coders_active,
                 burned_coder);
    pthread_mutex_unlock(&monitor->queue->mutex);
    return (NULL);
}

