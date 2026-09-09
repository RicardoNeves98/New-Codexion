#include "codexion.h"

int *display_error(char *inv_arg, int i)
{
    char *args[8] = {
        "number_of_coders", "time_to_burnout", "time_to_compile",
        "time_to_debug", "time_to_refactor", "number_of_compiles_required",
        "dongle_cooldown", "scheduler"
    };
    
    printf("[ERROR] '%s' is an invalid input for '%s' ", inv_arg, args[i]);
    if (i < 7)
        printf("(has to be a positive integer)\n");
    if (i == 7)
        printf("(has to be either 'fifo' or 'edf')\n");
    return (NULL);
}

int *parsing(int argc, char **argv)
{
    int i;
    int num;
    int *parsed_args;

    i = 0;
    if (argc != 9)
        return (printf("[ERROR] Needs 8 arguments to run\n"), NULL);
    parsed_args = malloc(8 * sizeof(*parsed_args));
    if (!parsed_args)
        return (NULL);
    while (++i < 8)
    {
        num = atoi(argv[i]);
        if (num <= 0)
            return (free(parsed_args), display_error(argv[i], i - 1));
        else
            parsed_args[i - 1] = num;
    }
    if (strcmp(argv[8], "fifo") == 0)
        parsed_args[7] = 0;
    else if (strcmp(argv[8], "edf") == 0)
        parsed_args[7] = 1;
    else
        return (free(parsed_args), display_error(argv[8], 7));
    return (parsed_args);
}
