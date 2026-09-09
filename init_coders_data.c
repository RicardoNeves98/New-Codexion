#include "codexion.h"

struct thread_vars *init_thread_vars(void)
{
    struct thread_vars *vars;

    vars = malloc(sizeof(*vars));
    if (!vars)
        return (NULL);
    if (pthread_mutex_init(&vars->mutex, NULL))
    {
        printf("Error initializing mutex variable\n");
        return (free(vars), NULL);
    }
    if (pthread_cond_init(&vars->cond, NULL))
    {
        pthread_mutex_destroy(&vars->mutex);
        printf("Error initializing condition variable\n");
        return (free(vars), NULL);
    }
    return (vars);
}

struct static_data *init_data(int *parsed_args)
{
    struct static_data *data;

    data = malloc(sizeof(*data));
    if (!data)
        return (printf("Error allocating memory\n"), NULL);
    data->coder_num = parsed_args[0];
    data->time_to_compile = parsed_args[2];
    data->time_to_debug = parsed_args[3];
    data->time_to_refactor = parsed_args[4];
    data->comp_required = parsed_args[5];
    data->cooldown = parsed_args[6];
    data->start_burnout = ms_to_timespec(parsed_args[1]);
    data->comp_burnout = add_time(data->start_burnout,
                                  ms_to_timespec(data->time_to_compile));
    data->max_wait = ms_to_timespec(parsed_args[2]);
    if (parsed_args[7] == 0)
        data->place_request = place_fifo_request;
    else if (parsed_args[7] == 1)
        data->place_request = place_edf_request;
    return (data);
}

int *init_line(void)
{
    int *line;

    line = malloc(2 * sizeof(*line));
    if (!line)
        return (NULL);
    line[0] = 0;
    line[1] = 0;
    return (line);
}   

struct dongle *init_dongles(int coder_num, struct timespec cooldown)
{
    int i;
    struct dongle *dongles;
    struct timespec now;

    i = -1;
    dongles = malloc(coder_num * sizeof(*dongles));
    if (!dongles)
        return (printf("Error allocation memory\n"), NULL);
    while (++i < coder_num)
    {
        clock_gettime(CLOCK_REALTIME, &now);
        dongles[i].next_aval = now;
        dongles[i].id = i + 1;
        dongles[i].is_free = 1;
        dongles[i].cooldown = cooldown;
        dongles[i].line = init_line();
        if (!dongles[i].line)
            return (free_dongles(dongles, i, -1), NULL);
        if (pthread_mutex_init(&dongles[i].mutex, NULL))
            return (free_dongles(dongles, i, 1), NULL);
    }
    return (dongles);
}

struct timespec *init_last_compile(int coder_num)
{
    int i;
    struct timespec *last_compile;

    i = -1;
    last_compile = malloc(coder_num * sizeof(*last_compile));
    if (!last_compile)
        return (printf("Error allocating memory\n"), NULL);
    while (++i < coder_num)
    {
        last_compile[i].tv_sec = 0;
        last_compile[i].tv_nsec = 0;
    }
    return (last_compile);
}

