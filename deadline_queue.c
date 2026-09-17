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

int get_end_spot(struct queue *deadline, struct timespec burnout, int on_queue,
                 int coder_num, int coder_id, int finished)
{
    int end;

    end = 0;
    if (on_queue == 0)
    {
        while (deadline[end].id != coder_id)
        {
            if (get_time_diff(burnout, deadline[end].time) < 0)
                break;
            end++;
        }
    }
    else
    {
        if (finished == 0)
            while (end < coder_num && deadline[end].id != 0 && deadline[end].id != -1)
                end++;
        else
            while (end < coder_num && deadline[end].id != -1)
                end++;
        end--;
    }
    return (end);
}

void deadline_update(struct queue *deadline, struct timespec burnout, int coder_num,
                     int coder_id, int finished)
{
    int on_queue;
    int start;
    int end;

    on_queue = 1;
    start = 0;
    while (start < (coder_num - 1) && deadline[start].id != coder_id)
        start++;
    if (deadline[start].id != coder_id)
    {
        on_queue = 0;
        start = 0;
        while (deadline[start].id != 0)
            start++;
        deadline[start].id = coder_id;
    }
    end = get_end_spot(deadline, burnout, on_queue, coder_num, coder_id, finished);
    if (finished == 1)
        deadline[start].id = -1;
    deadline[start].time = burnout;
    move_around(deadline, start, end);
}

