#include "codexion.h"

void display_error(char **argv, int inv_ind)
{
    char *args[8] = {
        "number_of_coders", "time_to_burnout", "time_to_compile",
        "time_to_debug", "time_to_refactor", "number_of_compiles_required",
        "dongle_cooldown", "scheduler"
    };
    
    printf("Error '%s' is invalid for '%s' ",
           argv[inv_ind], args[inv_ind - 1]);
    if (inv_ind < 8)
        printf("(only valid positive integers)\n");
    if (inv_ind == 8)
        printf("(only 'fifo' or 'edf')\n");
}

int parse_numbers(char **argv, int *parsed_args)
{
    int i;
    int j;

    i = 0;
    while (++i < 8)
    {
        j = 0;
        if (argv[i][0] == '0')
            return (i);
        while (argv[i][j])
        {
            if (argv[i][j] < '0' || argv[i][j] > '9')
                break ;
            j++;
        }
        if (argv[i][j] == '\0')
            parsed_args[i - 1] = atoi(argv[i]);
        else
            break ;
    }
    return (i);
}

int parse_scheduler(char *scheduler, int *parsed_args)
{
    if (strcmp(scheduler, "fifo") == 0)
    {
        parsed_args[7] = 0;
        return (1);
    }
    else if (strcmp(scheduler, "edf") == 0)
    {
        parsed_args[7] = 1;
        return (1);
    }
    return (0);
}

int *parsing(int argc, char **argv)
{
    int num;
    int *parsed_args;

    if (argc != 9)
    {
        printf("Error Needs 8 arguments to run\n");
        return (NULL);
    }
    parsed_args = malloc(8 * sizeof(*parsed_args));
    if (!parsed_args)
        return (NULL);
    num = parse_numbers(argv, parsed_args);
    if (1 <= num && num <= 7)
    {
        display_error(argv, num);
        return (free(parsed_args), NULL);
    }
    if (!parse_scheduler(argv[8], parsed_args))
    {
        display_error(argv, 8);
        return (free(parsed_args), NULL);
    }
    return (parsed_args);
}
