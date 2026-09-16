#include "codexion.h"

void fill_data(struct coders_state *coders, struct static_data *data,
               struct thread_vars *sync, struct dongle *dongles,
               struct timespec *last_compile)
{
    int i;

    i = -1;
    while (++i < data->coder_num)
    {
        coders[i].id = i + 1;
        coders[i].num_compiles = 0;
        coders[i].data = data;
        coders[i].left = &dongles[i]; 
        coders[i].right = &dongles[(i + 1) % data->coder_num];
        coders[i].sync = sync; 
        coders[i].last_compile = last_compile;
    }
}

int get_coders_data(int *parsed_args, struct coders_state *coders)
{
    struct static_data *data;
    struct thread_vars *sync;
    struct dongle *dongles;
    struct timespec *last_compile;

    data = init_data(parsed_args);
    if (!data)
        return (free(parsed_args), 0);
    free(parsed_args);
    sync = init_thread_vars();
    dongles = init_dongles(data->coder_num, ms_to_timespec(data->cooldown));
    last_compile = init_last_compile(data->coder_num, data->scheduler);
    data->dongles = dongles;
    fill_data(coders, data, sync, dongles, last_compile);
    if (!sync || !dongles || (!last_compile && data->scheduler == 1))
        return (0);
    return (1);
}

void share_data(struct coders_state *coders, struct monitor_state *monitor) 
{
    int i;

    i = -1;
    while (++i < coders->data->coder_num)
    {
        coders[i].error = monitor->error;
        coders[i].coders_active = monitor->coders_active;
        coders[i].deadline = monitor->deadline;
        coders[i].queue = monitor->queue;
        coders[i].output_mutex = monitor->output_mutex;
    }
}

int get_shared_data(struct coders_state *coders, struct monitor_state *monitor)
{
    int *error;
    int *coders_active;
    struct queue *deadline;
    struct thread_vars *queue;
    pthread_mutex_t *output_mutex;

    error = init_error();
    coders_active = init_coders_active();
    output_mutex = init_output_mutex();
    deadline = init_deadline(coders->data->coder_num);
    queue = init_thread_vars();
    monitor->error = error;
    monitor->coders_active = coders_active;
    monitor->deadline = deadline;
    monitor->queue = queue;
    monitor->output_mutex = output_mutex;
    share_data(coders, monitor);
    if (!error || !coders_active || !output_mutex || !deadline || !queue)
        return (0);
    return (1);
}

int get_data(int *parsed_args, struct coders_state *coders,
             struct monitor_state *monitor)
{
    int i;
    struct timespec now;
    struct timespec init_limit;

    i = -1;
    if (!get_coders_data(parsed_args, coders))
        return (0);
    if (!get_shared_data(coders, monitor))
        return (0);
    if (clock_gettime(CLOCK_MONOTONIC, &now) == -1 ||
        clock_gettime(CLOCK_REALTIME, &init_limit) == -1)
        return (0);
    init_limit.tv_sec += 1;
    while (++i < coders->data->coder_num)
    {
        coders[i].data->start_time = now;
        coders[i].init_limit = init_limit;
    }
    monitor->start_time = now;
    monitor->init_limit = init_limit;
    monitor->coder_num = coders->data->coder_num;
    return (1);
}

