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
    while (--index >= 0)
    {
        free(dongles[index].line);
        pthread_mutex_destroy(&dongles[index].mutex);
    }
    free(dongles);
}

void free_thread_vars(struct thread_vars *vars)
{
    if (vars)
    {
        pthread_mutex_destroy(&vars->mutex);
        pthread_cond_destroy(&vars->cond);
        free(vars);
    }
}

void free_coders_data(struct coders_state *coder)
{
    free(coder->error);
    free(coder->coders_active);
    if (coder->data && coder->data->dongles)
        free_dongles(coder->data->dongles, coder->data->coder_num, 0);
    free(coder->data);
    free(coder->deadline);
    free(coder->last_compile);
    if (coder->sync)
        free_thread_vars(coder->sync);
    if (coder->queue)
        free_thread_vars(coder->queue);
    if (coder->output_mutex)
        pthread_mutex_destroy(coder->output_mutex);
    free(coder->output_mutex);
}

void free_monitor_data(struct monitor_state *monitor)
{
    free(monitor->error);
    free(monitor->coders_active);
    free(monitor->deadline);
    if (monitor->queue)
        free_thread_vars(monitor->queue);
    if (monitor->output_mutex)
        pthread_mutex_destroy(monitor->output_mutex);
    free(monitor->output_mutex);
}

void free_all(struct coders_state *coder, struct monitor_state *monitor,
              pthread_t *threads)
{
    if (coder)
        free_coders_data(coder);
    else if (monitor)
        free_monitor_data(monitor);
    free(coder);
    free(monitor);
    free(threads);
}

