#include "codexion.h"

int main(int argc, char **argv)
{
    int i;
    int *parsed_args;
    struct coders_state *coders_info;
    struct monitor_state *monitor_info;
    pthread_t *threads;

    i = -1;
    parsed_args = parsing(argc, argv);
    if (!parsed_args)
        return (1);
    coders_info = malloc((parsed_args[0]) * sizeof(*coders_info));
    monitor_info = malloc(sizeof(*monitor_info));
    threads = malloc((parsed_args[0] + 1) * sizeof(*threads));
    if (!coders_info || !monitor_info || !threads)
    {
        printf("Error allocating memory\n");
        return (free_all(coders_info, monitor_info, threads), 1);
    }
    if (!init_threads(parsed_args, coders_info, monitor_info, threads))
        return (free_all(coders_info, monitor_info, threads), 1);
    while (++i <= coders_info->data->coder_num)
        pthread_join(threads[i], NULL);
    return (free_all(coders_info, monitor_info, threads), 0);
}

// Dont forget the malloc probably should be a calloc 
