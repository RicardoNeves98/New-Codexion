#include "codexion.h"

void place_fifo_request(int *line, int coder_id)
{
    if (!line[0])
        line[0] = coder_id;
    else
        line[1] = coder_id;
}

void place_edf_request(int *line, int coder_id, struct timespec *last_compile)
{
    int hold;
    struct timespec curr_last_comp;
    struct timespec other_last_comp;

    if (!line[0])
        line[0] = coder_id;
    else
    {
        line[1] = coder_id;
        other_last_comp = last_compile[line[0] - 1];
        curr_last_comp = last_compile[coder_id - 1];
        if (get_time_diff(curr_last_comp, other_last_comp) < 0)
        {
            hold = line[0];
            line[0] = line[1];
            line[1] = hold;
        }
    }
}

void place_request(int *line, int coder_id, struct timespec *last_compile)
{
    if (!last_compile)
        place_fifo_request(line, coder_id);
    else
        place_edf_request(line, coder_id, last_compile);
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

void print_lines(struct dongle *left, struct dongle *right)
{
    printf("Dongle%d Line -> %d %d\n", left->id, left->line[0], left->line[1]);
    printf("Dongle%d Line -> %d %d\n", right->id, right->line[0], right->line[1]);
}

