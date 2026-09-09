#include "codexion.h"

void make_request(struct dongle *left, struct dongle *right,
                  struct coders_state *coder_info)
{
    pthread_mutex_lock(&coder_info->sync->mutex);
    pthread_mutex_lock(&left->mutex);
    coder_info->data->place_request(left->line, coder_info);
    pthread_mutex_unlock(&left->mutex);
    pthread_mutex_lock(&right->mutex);
    coder_info->data->place_request(right->line, coder_info);
    pthread_mutex_unlock(&right->mutex);
    pthread_mutex_unlock(&coder_info->sync->mutex);
}

void delete_requests(struct dongle *left, struct dongle *right,
                     struct coders_state *coder_info)
{
    pthread_mutex_lock(&left->mutex);
    remove_requests(left->line, coder_info->id);
    pthread_mutex_unlock(&left->mutex);
    pthread_mutex_lock(&right->mutex);
    remove_requests(right->line, coder_info->id);
    pthread_mutex_unlock(&right->mutex);
    pthread_mutex_lock(&coder_info->sync->mutex);
    pthread_cond_broadcast(&coder_info->sync->cond);
    pthread_mutex_unlock(&coder_info->sync->mutex);
}
// Dont forget I had a waiting time so it would not request straight away 

void check_requests(struct dongle *left, struct dongle *right,
                    struct coders_state *coder_info)
{
    pthread_mutex_lock(&left->mutex);
    if (left->line[0] == coder_info->id)
    {
        pthread_mutex_unlock(&left->mutex);
        delete_requests(left, right, coder_info);
        return ;
    }
    pthread_mutex_unlock(&left->mutex);
    pthread_mutex_lock(&right->mutex);
    if (right->line[0] == coder_info->id)
    {
        pthread_mutex_unlock(&right->mutex);
        delete_requests(right, left, coder_info);
        return ;
    }
    pthread_mutex_unlock(&right->mutex);
}

int check_active(struct coders_state *coder_info)
{
    int state;

    state = 1;
    pthread_mutex_lock(&coder_info->queue->mutex);
    if (*coder_info->coders_active == 0)
        state = 0;
    else if (coder_info->num_compiles == coder_info->data->comp_required)
    {
        *coder_info->coders_active -= 1;
        pthread_cond_signal(&coder_info->queue->cond);
        state = 0;
    }
    pthread_mutex_unlock(&coder_info->queue->mutex);
    return (state);
}

void error_exit(struct coders_state *coder_info)
{
    pthread_mutex_lock(&coder_info->queue->mutex);
    *coder_info->coders_active = -1;
    pthread_cond_signal(&coder_info->queue->cond);
    pthread_mutex_unlock(&coder_info->queue->mutex);
}

void *coder_func(void *info)
{
    int got_dongles;
    struct coders_state *coder_info;

    coder_info = (struct coders_state *)info;
    update_deadline_queue(coder_info, 0);
    while (check_active(coder_info))
    {
        make_request(coder_info->left, coder_info->right, coder_info);
        got_dongles = get_both_dongles(coder_info->left, coder_info->right,
                                       coder_info);
        if (got_dongles == 1)
            go_work(coder_info);
        else if (got_dongles == 0)
            check_requests(coder_info->left, coder_info->right, coder_info);
        else
            error_exit(coder_info);
    }
    return (NULL);
}

