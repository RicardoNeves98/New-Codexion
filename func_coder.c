 #include "codexion.h"

void *error_exit(struct thread_vars *queue, int *error)
{
    pthread_mutex_lock(&queue->mutex);
    *error = 1;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
    return (NULL);
}

void make_request(struct dongle *left, struct dongle *right, int coder_id,
                  struct thread_vars *sync, struct timespec *last_compile)
{
    pthread_mutex_lock(&sync->mutex);
    pthread_mutex_lock(&left->mutex);
    place_request(left->line, coder_id, last_compile);
    pthread_mutex_unlock(&left->mutex);
    pthread_mutex_lock(&right->mutex);
    place_request(right->line, coder_id, last_compile);
    pthread_mutex_unlock(&right->mutex);
    pthread_mutex_unlock(&sync->mutex);
}

int check_active(struct coders_state *coder)
{
    int state;

    state = 1;
    pthread_mutex_lock(&coder->queue->mutex);
    if (*coder->coders_active == 0 || *coder->error == 1)
        state = 0;
    else if (coder->num_compiles == coder->data->comp_required)
    {
        *coder->coders_active -= 1;
        pthread_cond_signal(&coder->queue->cond);
        state = 0;
    }
    pthread_mutex_unlock(&coder->queue->mutex);
    return (state);
}

int get_dongles_and_compile(struct coders_state *coder)
{
    int dongles_state;

    dongles_state = get_dongles(coder);
    if (dongles_state == 0)
    {
        if (check_requests(coder->left, coder->right, coder->id, coder->sync))
            return (1);
        else
            return (0);
    }
    else if (dongles_state == 1)
    {
        if (!go_work(coder))
            return (-1);
        else
            return (1);
    }
    else
        return (-1);
}

void *coder_func(void *info)
{
    int has_request;
    struct coders_state *coder;

    has_request = 0;
    coder = (struct coders_state *)info;
    if (!check_all(coder->queue, coder->coders_active, coder->error,
                   coder->data->coder_num, coder->init_limit))
        return (NULL);
    if (!update_deadline_queue(coder, 0))
        return (error_exit(coder->queue, coder->error));
    while (check_active(coder))
    {
        if (!has_request)
            make_request(coder->left, coder->right, coder->id, coder->sync,
                         coder->last_compile);
        has_request = get_dongles_and_compile(coder);
        if (has_request == -1)
            error_exit(coder->queue, coder->error);
    }
    return (NULL);
}

