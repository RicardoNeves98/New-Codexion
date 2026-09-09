#include "codexion.h"

int check_free(struct dongle *left, struct dongle *right, int coder_id)
{
    int dongles_aval;

    dongles_aval = 0;
    pthread_mutex_lock(&left->mutex); 
    if (left->line[0] == coder_id && left->is_free)
        dongles_aval += 1;
    pthread_mutex_unlock(&left->mutex);
    pthread_mutex_lock(&right->mutex);
    if (right->line[0] == coder_id && right->is_free)
        dongles_aval += 1;
    pthread_mutex_unlock(&right->mutex);
    return (dongles_aval);
}

long lock_dongle(struct dongle *curr_dongle)
{
    long dongle_wait;
    struct timespec now;

    pthread_mutex_lock(&curr_dongle->mutex);
    curr_dongle->is_free = 0;
    update_line(curr_dongle->line);
    clock_gettime(CLOCK_REALTIME, &now);
    dongle_wait = get_time_diff(curr_dongle->next_aval, now);
    if (dongle_wait < 0)
        dongle_wait = 0;
    pthread_mutex_unlock(&curr_dongle->mutex);
    return (dongle_wait);
}

void print_message(struct coders_state *coder_info)
{
    struct timespec now;

    pthread_mutex_lock(&coder_info->queue->mutex);
    if (*coder_info->coders_active > 0)
    {
        pthread_mutex_lock(coder_info->output_mutex);
        clock_gettime(CLOCK_MONOTONIC, &now);
        printf("%ld %d has taken dongle\n",
               get_time_diff(now, coder_info->data->start_time), coder_info->id);
        pthread_mutex_unlock(coder_info->output_mutex);
    }
    pthread_mutex_unlock(&coder_info->queue->mutex);
}

void wait_and_print(long left_wait, long right_wait, struct coders_state *coder_info)
{
    if (left_wait <= right_wait)
    {
        usleep(left_wait);
        print_message(coder_info);
        usleep(right_wait);
        print_message(coder_info);
    }
    else
    {
        usleep(right_wait);
        print_message(coder_info);
        usleep(left_wait);
        print_message(coder_info);
    }
}

int get_both_dongles(struct dongle *left, struct dongle *right,
                     struct coders_state *coder_info)
{
    int value;
    int dongles_free;
    long left_wait;
    long right_wait;
    struct timespec time_limit;

    pthread_mutex_lock(&coder_info->sync->mutex);
    time_limit = add_curr_time(coder_info->data->max_wait);
    dongles_free = check_free(left, right, coder_info->id);
    while (dongles_free != 2)
    {
        value = pthread_cond_timedwait(&coder_info->sync->cond,
                                       &coder_info->sync->mutex, &time_limit);
        if (value == ETIMEDOUT)
            return (pthread_mutex_unlock(&coder_info->sync->mutex), 0);
        else if (value != 0)
            return (pthread_mutex_unlock(&coder_info->sync->mutex), -1);
        dongles_free = check_free(left, right, coder_info->id);
    }
    left_wait = lock_dongle(left);
    right_wait = lock_dongle(right);
    pthread_mutex_unlock(&coder_info->sync->mutex);
    wait_and_print(left_wait, right_wait, coder_info);
    return (1);
}

