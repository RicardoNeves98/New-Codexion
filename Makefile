CFLAGS = -Wall -Wextra -Werror -g
 
SRCS = compile.c deadline_queue.c free_stuff.c func_coder.c func_monitor.c \
        get_dongle.c get_structs_data.c init_coders_structs.c init_shared_structs.c \
        init_structs_and_threads.c  line_requests.c main.c parsing.c \
        request_utils.c time_utils.c shared_utils.c

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
