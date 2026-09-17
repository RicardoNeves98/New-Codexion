#include "codexion.h"

void delete_requests(struct dongle *left, struct dongle *right, int coder_id,
                     struct thread_vars *sync)
{
    if (left)
    {
        pthread_mutex_lock(&left->mutex);
        remove_requests(left->line, coder_id);
        pthread_mutex_unlock(&left->mutex);
    }
    if (right)
    {
        pthread_mutex_lock(&right->mutex);
        remove_requests(right->line, coder_id);
        pthread_mutex_unlock(&right->mutex);
    }
    pthread_mutex_lock(&sync->mutex);
    pthread_cond_broadcast(&sync->cond);
    pthread_mutex_unlock(&sync->mutex);
}
// Dont forget I had a waiting time so it would not request straight away 

int check_requests(struct dongle *left, struct dongle *right, int coder_id,
                   struct thread_vars *sync)
{
    pthread_mutex_lock(&left->mutex);
    if (left || left->line[0] == coder_id)
    {
        pthread_mutex_unlock(&left->mutex);
        delete_requests(left, right, coder_id, sync);
        return (1);
    }
    pthread_mutex_unlock(&left->mutex);
    pthread_mutex_lock(&right->mutex);
    if (right || right->line[0] == coder_id)
    {
        pthread_mutex_unlock(&right->mutex);
        delete_requests(right, left, coder_id, sync);
        return (1);
    }
    pthread_mutex_unlock(&right->mutex);
    return (0);
}

