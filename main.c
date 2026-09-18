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
// -> Test is parsing is working well CHECK 
// -> Test bad allocation in coder, monitor or threads CHECK 
// -> Test bad allocation in structs for coder and monitor data CHECK 
// -> Test fail in initializing threads CHECK 
// -> Test error in pthread_cond_{wait, timedwait} and clock_gettime CHECK 
// -> Check if the lines are good and if the deadline is also well 

// Important Notes: 
// 1) Codexion says 'Reject invalid such as negative numbers, non-integers (...)' 
// but I also reject 0 as a value 
// 2) In the way I am coding there is no possible deadlock but to improve liveness 
// I remove requests sometimes and need to make sure that matches fairness rules 
// 3) Need to check if the need to implement the heap queue for scheduling needs 
// to be a certain way and have certain types of functions 
// 4) Probably valid but I do things in a way that looks different to the output 
// because I wait for dongles to be free and once they are I claim them but only 
// hold them once they passed cooldown period that is why in most situations I 
// grab dongles not simultaneously 

