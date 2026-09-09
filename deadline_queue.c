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
    struct timespec coder_burnout;

    coder_burnout = add_curr_time(burnout);
    start = 0;
    end = 0;
    while (deadline[start].id != 0)
        start++;
    while (deadline[end].id != 0)
    {
        if (get_time_diff(coder_burnout, deadline[end].time) < 0)
            break;
        end++;
    }
    deadline[start].id = coder_id; 
    deadline[start].time = coder_burnout;
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
    deadline[start].time = add_curr_time(burnout);
    move_around(deadline, start, end);
}

void print_deadline(struct queue *deadline, int coder_num)
{
    int i;

    i = -1;
    while (++i < coder_num)
        printf("Spot %d -> Coder %d Time %ld sec %ld nsec\n",
               i, deadline[i].id, deadline[i].time.tv_sec, deadline[i].time.tv_nsec);
}
