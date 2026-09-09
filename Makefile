CFLAGS = -Wall -Wextra -Werror

SRCS = compile.c deadline_queue.c free_stuff.c func_coder.c func_monitor.c \
        get_dongle.c init_coders_data.c init_shared_data.c init_threads.c \
        line_requests.c main.c parsing.c time_conversion.c time_utils.c

OBJS = $(SRCS:.c=.o)

all: $(OBJS)
	cc $(CFLAGS) -pthread -o program $(OBJS)

debug: $(OBJS)
	gcc -g $(CFLAGS) -pthread -o program $(OBJS)

%.o: %.c
	cc $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f program

re: fclean all

.PHONY: all debug clean fclean re
