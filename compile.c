#include "codexion.h"

void update_deadline_queue(struct coders_state *coder_info, int compile)
{
    int finished;

    pthread_mutex_lock(&coder_info->queue->mutex);
    if (compile == 1)
    {
        finished = 0;
        if (coder_info->num_compiles == coder_info->data->comp_required)
            finished = 1;
        comp_deadline_update(coder_info->deadline, coder_info->data->coder_num,
                             coder_info->id, coder_info->data->comp_burnout, finished);
    }
    else
        first_deadline_update(coder_info->deadline, coder_info->id,
                              coder_info->data->start_burnout);
    pthread_cond_signal(&coder_info->queue->cond);
    pthread_mutex_unlock(&coder_info->queue->mutex);
}

void update_dongle_state(struct dongle *curr_dongle)
{
    pthread_mutex_lock(&curr_dongle->mutex);
    curr_dongle->is_free = 1;
    curr_dongle->next_aval = add_curr_time(curr_dongle->cooldown);
    pthread_mutex_unlock(&curr_dongle->mutex);
}

void write_output(char *type, struct coders_state *coder_info)
{
    struct timespec now;

    pthread_mutex_lock(&coder_info->queue->mutex);
    if (*coder_info->coders_active > 0)
    {
        pthread_mutex_lock(coder_info->output_mutex);
        clock_gettime(CLOCK_MONOTONIC, &now);
        printf("%ld %d is %s\n", get_time_diff(now, coder_info->data->start_time),
               coder_info->id, type);
        pthread_mutex_unlock(coder_info->output_mutex);
    }
    pthread_mutex_unlock(&coder_info->queue->mutex);
}

void go_work(struct coders_state *coder_info)
{
    struct timespec now;

    coder_info->num_compiles += 1;
    update_deadline_queue(coder_info, 1);
    write_output("compiling", coder_info);
    usleep(coder_info->data->time_to_compile * 1000);
    clock_gettime(CLOCK_REALTIME, &now);
    update_dongle_state(coder_info->left);
    update_dongle_state(coder_info->right);
    pthread_mutex_lock(&coder_info->sync->mutex);
    pthread_cond_broadcast(&coder_info->sync->cond);
    coder_info->last_compile[coder_info->id - 1] = now;
    pthread_mutex_unlock(&coder_info->sync->mutex);
    write_output("debugging", coder_info);
    usleep(coder_info->data->time_to_debug * 1000);
    write_output("refactoring", coder_info);
    usleep(coder_info->data->time_to_refactor * 1000);
}

// Check if change in the last_compile does not need to be earlier 
// I dont currently think so because it is only used in edf line and by this 
// Time the coder has no request in line and will only make another request after
