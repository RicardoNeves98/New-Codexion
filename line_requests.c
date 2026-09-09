#include "codexion.h"

void place_fifo_request(int *line, struct coders_state *coder_info)
{
    if (!line[0])
        line[0] = coder_info->id;
    else
        line[1] = coder_info->id;
}

void place_edf_request(int *line, struct coders_state *coder_info)
{
    int hold;    
    struct timespec coder0_last_comp;
    struct timespec coder1_last_comp;

    if (!line[0])
        line[0] = coder_info->id;
    else
    {
        line[1] = coder_info->id;
        coder0_last_comp = coder_info->last_compile[line[0] - 1];
        coder1_last_comp = coder_info->last_compile[coder_info->id - 1];
        if (get_time_diff(coder1_last_comp, coder0_last_comp) < 0)
        {
            hold = line[0];
            line[0] = line[1];
            line[1] = hold;
        }
    }
}

void update_line(int *line)
{
    line[0] = line[1];
    line[1] = 0;
}

void remove_requests(int *line, int coder_id)
{
    if (line[0] == coder_id)
        update_line(line);
    else if (line[1] == coder_id)
        line[1] = 0;
}

void print_lines(struct dongle *left, struct dongle *right,
                 pthread_mutex_t *output_mutex)
{
    pthread_mutex_lock(output_mutex);
    printf("Dongle%d Line -> %d %d\n", left->id, left->line[0], left->line[1]);
    printf("Dongle%d Line -> %d %d\n", right->id, right->line[0], right->line[1]);
    pthread_mutex_unlock(output_mutex);
}  


