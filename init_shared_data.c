#include "codexion.h"

int *init_coders_active(int coder_num)
{
    int *coders_active;

    coders_active = malloc(sizeof(*coders_active));
    if (!coders_active)
        return (printf("Error allocating memory\n"), NULL);
    *coders_active = coder_num;
    return (coders_active);
}

pthread_mutex_t *init_output_mutex(void)
{
    pthread_mutex_t *output_mutex;

    output_mutex = malloc(sizeof(*output_mutex));
    if (!output_mutex)
        return (printf("Error allocating memory\n"), NULL);
    if (pthread_mutex_init(output_mutex, NULL))
    {
        printf("Error initizing mutex variable\n");
        return (free(output_mutex), NULL);
    }
    return (output_mutex);
}

struct queue *init_deadline(int coder_num, struct timespec start_burnout)
{
    int i;
    struct queue *deadline;

    i = -1;
    deadline = malloc(coder_num * sizeof(*deadline));
    if (!deadline)
        return (printf("Error allocating memory\n"), NULL);
    while (++i < coder_num)
    {
        deadline[i].id = 0;
        deadline[i].time = add_curr_time(start_burnout);
    }
    return (deadline);
}
 
