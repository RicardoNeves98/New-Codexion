#include "codexion.h"

void free_dongles(struct dongle *dongles, int index, int line)
{
    if (line == -1)
        printf("Error allocating memory\n");
    else if (line == 1)
    {
        printf("Error initializing mutex\n");
        free(dongles[index].line);
    }
    else
    {
        while (--index >= 0)
        {
            pthread_mutex_destroy(&dongles[index].mutex);
            free(dongles[index].line);
        }
        free(dongles);
    }
}

void free_coder_data(struct static_data *data, struct thread_vars *sync,
                     struct timespec *last_compile)
{
    if (sync)
    {
        pthread_mutex_destroy(&sync->mutex);
        pthread_cond_destroy(&sync->cond);
        free(sync);
    }
    if (data && data->dongles)
        free_dongles(data->dongles, data->coder_num, 0);
    if (last_compile)
        free(last_compile);
    if (data)
        free(data);
}

void free_shared_data(int *coders_active, pthread_mutex_t *output_mutex,
                      struct queue *deadline, struct thread_vars *queue)
{
    if (coders_active)
        free(coders_active);
    if (output_mutex)
    {
        pthread_mutex_destroy(output_mutex);
        free(output_mutex);
    }
    if (deadline)
        free(deadline);
    if (queue)
    {
        pthread_mutex_destroy(&queue->mutex);
        pthread_cond_destroy(&queue->cond);
        free(queue);
    }
}

void free_all(struct coders_state *coder_info, struct monitor_state *monitor_info,
              pthread_t *threads)
{
    if (coder_info)
    {
        free_coder_data(coder_info->data, coder_info->sync, coder_info->last_compile);
        free_shared_data(coder_info->coders_active, coder_info->output_mutex,
                         coder_info->deadline, coder_info->queue);
    }
    else if (monitor_info)
        free_shared_data(monitor_info->coders_active, monitor_info->output_mutex,
                         monitor_info->deadline, monitor_info->queue);
    free(coder_info);
    free(monitor_info);
    free(threads);
}

