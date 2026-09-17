#include "codexion.h"

int check_free(struct dongle *left, struct dongle *right, int coder_id)
{
    int dongles_aval;

    dongles_aval = 0;
    if (left)
    {
        pthread_mutex_lock(&left->mutex);
        if (left->line[0] == coder_id && left->is_free)
            dongles_aval += 1;
        pthread_mutex_unlock(&left->mutex);
    }
    if (right)
    {
        pthread_mutex_lock(&right->mutex);
        if (right->line[0] == coder_id && right->is_free)
            dongles_aval += 1;
        pthread_mutex_unlock(&right->mutex);
    }
    return (dongles_aval);
}

int wait_and_write(struct timespec left_aval, struct timespec right_aval,
                   struct coders_state *coder)
{
    long left_wait;
    long right_wait;
    long min_wait;
    long max_wait;
    struct timespec now;

    if (clock_gettime(CLOCK_MONOTONIC, &now) == -1)
        return (0);
    left_wait = get_time_diff(left_aval, now);
    right_wait = get_time_diff(right_aval, now);
    if (left_wait < 0)
        left_wait = 0;
    if (right_wait < 0)
        right_wait = 0;
    min_wait = get_min_time(left_wait, right_wait);
    max_wait = get_max_time(left_wait, right_wait);
    usleep(min_wait * 1000);
    if (!write_message(coder, "has taken a dongle"))
        return (0);
    usleep((max_wait - min_wait) * 1000);
    if (!write_message(coder, "has taken a dongle"))
        return (0);
    return (1);
}

struct timespec lock_dongle(struct dongle *curr_dongle)
{
    struct timespec dongle_aval;

    pthread_mutex_lock(&curr_dongle->mutex);
    curr_dongle->is_free = 0;
    update_line(curr_dongle->line);
    dongle_aval = curr_dongle->next_aval;
    pthread_mutex_unlock(&curr_dongle->mutex);
    return (dongle_aval);
}

int wait_aval(struct dongle *left, struct dongle *right, int coder_id,
              struct thread_vars *sync, struct timespec max_wait)
{
    int value;
    struct timespec now;
    struct timespec time_limit;

    if (clock_gettime(CLOCK_REALTIME, &now) == -1)
        return (-1);
    time_limit = add_time(now, max_wait);
    while (check_free(left, right, coder_id) < 2)
    {
        value = pthread_cond_timedwait(&sync->cond, &sync->mutex, &time_limit);
        if (value == ETIMEDOUT)
            return (0);
        else if (value != 0)
            return (-1);
    }
    return (1);
}

int get_dongles(struct coders_state *coder)
{
    int aval;
    struct timespec left_aval;
    struct timespec right_aval;

    pthread_mutex_lock(&coder->sync->mutex);
    aval = wait_aval(coder->left, coder->right, coder->id,
                     coder->sync, coder->data->max_wait);
    if (aval == -1 || aval == 0)
    {
        pthread_mutex_unlock(&coder->sync->mutex);
        return (aval);
    }
    left_aval = lock_dongle(coder->left);
    right_aval = lock_dongle(coder->right);
    pthread_mutex_unlock(&coder->sync->mutex);
    if (!wait_and_write(left_aval, right_aval, coder))
        return (-1);
    return (1);
}

