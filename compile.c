#include "codexion.h"

int update_deadline_queue(struct coders_state *coder, int compile)
{
    pthread_mutex_lock(&coder->queue->mutex);
    if (!deadline_update(coder, compile))
    {
        pthread_mutex_unlock(&coder->queue->mutex);
        return (0);
    }
    pthread_cond_signal(&coder->queue->cond);
    pthread_mutex_unlock(&coder->queue->mutex);
    return (1);
}

int update_dongles_state(struct dongle *left, struct dongle *right)
{
    struct timespec now;
    struct timespec next_aval;

    if (clock_gettime(CLOCK_REALTIME, &now) == -1)
        return (0);
    next_aval = add_time(now, left->cooldown);
    pthread_mutex_lock(&left->mutex);
    left->is_free = 1;
    left->next_aval = next_aval;
    pthread_mutex_unlock(&left->mutex);
    pthread_mutex_lock(&right->mutex);
    right->is_free = 1;
    right->next_aval = next_aval;
    pthread_mutex_unlock(&right->mutex);
    return (1);
}

void signal_and_update(struct coders_state *coder, struct timespec now)
{
    pthread_mutex_lock(&coder->sync->mutex);
    pthread_cond_broadcast(&coder->sync->cond);
    if (coder->last_compile)
        coder->last_compile[coder->id - 1] = now;
    pthread_mutex_unlock(&coder->sync->mutex);
}

int go_work(struct coders_state *coder)
{
    struct timespec now;

    coder->num_compiles += 1;
    if (!update_deadline_queue(coder, 1))
        return (0);
    if (!write_message(coder, "is compiling"))
        return (0);
    usleep(coder->data->time_to_compile * 1000);
    if (clock_gettime(CLOCK_REALTIME, &now) == -1)
        return (0);
    if (!update_dongles_state(coder->left, coder->right))
        return (0);
    signal_and_update(coder, now);
    if (!write_message(coder, "is debugging"))
        return (0);
    usleep(coder->data->time_to_debug * 1000);
    if (!write_message(coder, "is refactoring"))
        return (0);
    usleep(coder->data->time_to_refactor * 1000);
    return (1);
}

// Check if change in the last_compile does not need to be earlier 
// I dont currently think so because it is only used in edf line and by this 
// Time the coder has no request in line and will only make another request after
