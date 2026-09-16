#include "codexion.h"

void move_around(struct queue *deadline, int start, int end)
{
    int dir;
    int int_hold;
    struct timespec time_hold;

    dir = 1;
    if (end < start)
        dir = -1;
    while (start != end)
    {
        int_hold = deadline[start].id;
        time_hold = deadline[start].time;
        deadline[start].id = deadline[start + dir].id;
        deadline[start].time = deadline[start + dir].time;
        deadline[start + dir].id = int_hold;
        deadline[start + dir].time = time_hold;
        start += dir;
    }
}

void first_deadline_update(struct queue *deadline, int coder_id,
                           struct timespec burnout)
{
    int start;
    int end;

    start = 0;
    end = 0;
    while (deadline[start].id != 0)
        start++;
    while (deadline[end].id != 0)
    {
        if (get_time_diff(burnout, deadline[end].time) < 0)
            break;
        end++;
    }
    deadline[start].id = coder_id; 
    deadline[start].time = burnout;
    move_around(deadline, start, end);
}

void comp_deadline_update(struct queue *deadline, int coder_num, int coder_id,
                          struct timespec burnout, int finished)
{
    int start;
    int end;

    start = 0;
    end = 0;
    while (deadline[start].id != coder_id)
        start++;
    while (end < (coder_num - 1) && deadline[end].id != 0 && deadline[end].id != -1)
        end++;
    if (finished == 1 && deadline[end].id == 0)
        while (end < (coder_num - 1) && deadline[end].id != -1)
            end++;
    if (finished == 1)
        deadline[start].id = -1;
    deadline[start].time = burnout;
    move_around(deadline, start, end);
}

int deadline_update(struct coders_state *coder, int compile)
{
    int finished;
    struct timespec now;
    struct timespec burnout;

    finished = 0;
    if (clock_gettime(CLOCK_REALTIME, &now) == -1)
        return (0);
    if (compile)
    {
        burnout = add_time(now, coder->data->comp_burnout);
        if (coder->num_compiles == coder->data->comp_required)
            finished = 1;
        comp_deadline_update(coder->deadline, coder->data->coder_num,
                             coder->id, burnout, finished);
    }
    else
    {
        burnout = add_time(now, coder->data->start_burnout);
        first_deadline_update(coder->deadline, coder->id, burnout);
    }
    return (1);
}

void print_deadline(struct queue *deadline, int coder_num)
{
    int i;

    i = -1;
    while (++i < coder_num)
        printf("Spot %d -> Coder %d Time %ld sec %ld nsec\n",
               i, deadline[i].id, deadline[i].time.tv_sec, deadline[i].time.tv_nsec);
}
