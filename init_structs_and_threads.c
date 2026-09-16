#include "codexion.h"

struct coders_state *init_coders_struct(int coder_num)
{
    int i;
    struct coders_state *coders;

    i = -1;
    coders = malloc(coder_num * sizeof(*coders));
    if (!coders)
        return (NULL);
    while (++i < coder_num)
    {
        coders[i].error = NULL;
        coders[i].coders_active = NULL;
        coders[i].left = NULL;
        coders[i].right = NULL;
        coders[i].data = NULL;
        coders[i].deadline = NULL;
        coders[i].last_compile = NULL;
        coders[i].sync = NULL;
        coders[i].queue = NULL;
        coders[i].output_mutex = NULL;
    }
    return (coders);
}

struct monitor_state *init_monitor_struct(void)
{
    struct monitor_state *monitor;

    monitor = malloc(sizeof(*monitor));
    if (!monitor)
        return (NULL);
    monitor->error = NULL;
    monitor->coders_active = NULL;
    monitor->deadline = NULL;
    monitor->queue = NULL;
    monitor->output_mutex = NULL;
    return (monitor);
}

void init_threads(struct coders_state *coders, struct monitor_state *monitor,
                  pthread_t *threads)
{
    int i;
    int array[coders->data->coder_num];

    i = 0;
    array[i] = 1;
    if (pthread_create(&threads[i], NULL, monitor_func, (void *)monitor))
    {
        printf("Error initializing monitor thread\n");
        array[i] = 0;
    }
    while (++i <= coders->data->coder_num)
    {
        array[i] = 1;
        if (pthread_create(&threads[i], NULL, coder_func, (void *)&coders[i - 1]))
        {
            printf("Error initializing coder %d thread\n", i);
            array[i] = 0;
        }
    }
    i = -1;
    while (++i <= coders->data->coder_num)
        if (array[i] == 1)
            pthread_join(threads[i], NULL);
}

