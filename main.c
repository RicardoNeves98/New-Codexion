#include "codexion.h"

int main(int argc, char **argv)
{
    int *parsed_args;
    struct coders_state *coders;
    struct monitor_state *monitor;
    pthread_t *threads;

    parsed_args = parsing(argc, argv);
    if (!parsed_args)
        return (1);
    coders = init_coders_struct(parsed_args[0]);
    monitor = init_monitor_struct();
    threads = malloc((parsed_args[0] + 1) * sizeof(*threads));
    if (!coders || !monitor || !threads)
    {
        printf("Error allocating memory\n");
        return (free_all(coders, monitor, threads), free(parsed_args), 1);
    }
    if (!get_data(parsed_args, coders, monitor))
        return (free_all(coders, monitor, threads), 1);
    init_threads(coders, monitor, threads);
    return (free_all(coders, monitor, threads), 0);
}

// Testing cases 
// -> Test is parsing is working well 
// -> Test bad allocation in coder, monitor or threads CHECK 
// -> Test bad allocation in structs for coder and monitor data CHECK
// -> Test fail in initializing threads CHECK 
// -> Test error in pthread_cond_{wait, timedwait} and clock_gettime
// -> Check if the lines are good and if the deadline is also well 
