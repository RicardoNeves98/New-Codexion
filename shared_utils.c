#include "codexion.h"

int write_message(struct coders_state *coder, char *action)
{
    int keep_going;
    struct timespec now;

    keep_going = 1;
    pthread_mutex_lock(&coder->queue->mutex);
    if (*coder->error == 1)
        keep_going = 0;
    else if (*coder->coders_active > 0) 
    {
        pthread_mutex_lock(coder->output_mutex);
        if (clock_gettime(CLOCK_MONOTONIC, &now) == -1)
            keep_going = 0;
        else
            printf("%ld %d %s\n", get_time_diff(now, coder->data->start_time),
                   coder->id, action);
        pthread_mutex_unlock(coder->output_mutex);
    }
    pthread_mutex_unlock(&coder->queue->mutex);
    return (keep_going);
}

int check_all(struct thread_vars *queue, int *coders_active, int *error,
              int coder_num, struct timespec init_limit)
{
    int value;

    pthread_mutex_lock(&queue->mutex);
    *coders_active += 1;
    pthread_cond_broadcast(&queue->cond);
    while (*coders_active < coder_num && *error == 0)
    {
        value = pthread_cond_timedwait(&queue->cond, &queue->mutex, &init_limit);
        if (value == ETIMEDOUT)
        {
            pthread_mutex_unlock(&queue->mutex);
            return (0);
        }
        else if (value != 0)
        {
            *error = 1;
            pthread_cond_broadcast(&queue->cond);
            pthread_mutex_unlock(&queue->mutex);
            return (0);
        }
    }
    if (*error == 1)
    {
        pthread_mutex_unlock(&queue->mutex);
        return (0);
    }
    pthread_mutex_unlock(&queue->mutex);
    return (1);
}

