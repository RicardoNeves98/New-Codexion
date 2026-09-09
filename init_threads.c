#include "codexion.h"

void fill_coders_data1(struct coders_state *coders_info, struct static_data *data,
                       struct thread_vars *sync, struct dongle *dongles,
                       struct timespec *last_compile)
{
    int i;

    i = -1;
    while (++i < data->coder_num)
    {
        coders_info[i].id = i + 1;
        coders_info[i].num_compiles = 0;
        coders_info[i].data = data;
        coders_info[i].left = &dongles[i]; 
        coders_info[i].right = &dongles[(i + 1) % data->coder_num];
        coders_info[i].sync = sync; 
        coders_info[i].last_compile = last_compile;
    }
}

int get_coders_data(int *parsed_args, struct coders_state *coders_info)
{
    struct static_data *data;
    struct thread_vars *sync;
    struct dongle *dongles;
    struct timespec *last_compile;

    data = init_data(parsed_args);
    if (!data)
        return (0);
    sync = init_thread_vars();
    dongles = init_dongles(data->coder_num, ms_to_timespec(data->cooldown));
    last_compile = init_last_compile(data->coder_num);
    data->dongles = dongles;
    if (!sync || !dongles || !last_compile)
        return (free_coder_data(data, sync, last_compile), 0);
    fill_coders_data1(coders_info, data, sync, dongles, last_compile);   
    return (1);
}

void fill_coders_data2(struct coders_state *coders_info, int *coders_active,
                       struct queue *deadline, struct thread_vars *queue,
                       pthread_mutex_t *output_mutex)
{
    int i;

    i = -1;
    while (++i < coders_info->data->coder_num)
    {
        coders_info[i].coders_active = coders_active;
        coders_info[i].deadline = deadline;
        coders_info[i].queue = queue;
        coders_info[i].output_mutex = output_mutex;
    }
}

int get_shared_data(struct coders_state *coders_info,
                    struct monitor_state *monitor_info)
{
    int *coders_active;
    struct queue *deadline;
    struct thread_vars *queue;
    pthread_mutex_t *output_mutex;

    coders_active = init_coders_active(coders_info->data->coder_num);
    output_mutex = init_output_mutex();
    deadline = init_deadline(coders_info->data->coder_num,
                             coders_info->data->start_burnout);
    queue = init_thread_vars();
    if (!coders_active || !output_mutex || !deadline || !queue)
        return (free_shared_data(coders_active, output_mutex, deadline, queue), 0);
    fill_coders_data2(coders_info, coders_active, deadline, queue, output_mutex);
    monitor_info->coders_active = coders_active;
    monitor_info->deadline = deadline;
    monitor_info->queue = queue;
    monitor_info->output_mutex = output_mutex;
    return (1);
}

int init_threads(int *parsed_args, struct coders_state *coders_info,
                 struct monitor_state *monitor_info, pthread_t *threads)
{
    int i;
    struct timespec now;

    i = 0;
    if (!get_coders_data(parsed_args, coders_info))
        return (free(parsed_args), 0);
    free(parsed_args);
    if (!get_shared_data(coders_info, monitor_info))
        return (0);
    clock_gettime(CLOCK_MONOTONIC, &now);
    coders_info->data->start_time = now;
    monitor_info->start_time = now;
    if (pthread_create(&threads[i], NULL, monitor_func, (void *)monitor_info))
        return (printf("Error creating monitor thread\n"), 0);
    while (++i <= coders_info->data->coder_num)
        if (pthread_create(&threads[i], NULL, coder_func, (void *)&coders_info[i - 1]))
            return (printf("Error creating coder thread\n"), 0);
    return (1);
}

