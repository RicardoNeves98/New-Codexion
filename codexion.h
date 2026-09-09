#ifndef CODEXION_H
# define CODEXION_H
#endif

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

typedef struct coders_state
{
    int id;
    int num_compiles;
    int *coders_active;
    struct dongle *left;
    struct dongle *right;
    struct static_data *data;
    struct queue *deadline;
    struct timespec *last_compile;
    struct thread_vars *sync;
    struct thread_vars *queue;
    pthread_mutex_t *output_mutex;
}   coder_thread;

typedef struct static_data
{
    int coder_num;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int comp_required;
    int cooldown;
    struct timespec start_burnout;
    struct timespec comp_burnout;
    struct timespec max_wait;
    struct timespec start_time;
    struct dongle *dongles;
    void (*place_request)(int *line, struct coders_state *coder_info);
}   static_data;

typedef struct thread_vars
{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}   thread_vars;

typedef struct queue
{
    int id;
    struct timespec time;
}   queue;

typedef struct dongle
{
    int id;
    int is_free;
    int *line;
    struct timespec cooldown;
    struct timespec next_aval;
    pthread_mutex_t mutex;
}   dongle;

typedef struct monitor_state
{
    int id;
    struct timespec start_time;
    int *coders_active;
    struct queue *deadline;
    struct thread_vars *queue;
    pthread_mutex_t *output_mutex;
}   monitor_thread;

// compile.c
void update_deadline_queue(struct coders_state *coder_info, int compile);
void update_dongle_state(struct dongle *curr_dongle);
int check_coders(struct coders_state *coder_info);
void write_output(char *type, struct coders_state *coder_info);
void go_work(struct coders_state *coder_info);

// deadline_queue.c
void move_around(struct queue *deadline, int start, int end);
void first_deadline_update(struct queue *deadline, int coder_id,
                           struct timespec burnout);
void comp_deadline_update(struct queue *deadline, int coder_num, int coder_id,
                          struct timespec burnout, int finished);
void print_deadline(struct queue *deadline, int coder_num);

// free_stuff.c
void free_dongles(struct dongle *dongles, int index, int line);
void free_coder_data(struct static_data *data, struct thread_vars *sync,
                     struct timespec *last_compile);
void free_shared_data(int *coders_active, pthread_mutex_t *output_mutex,
                      struct queue *deadline, struct thread_vars *queue);
void free_all(struct coders_state *coder_info, struct monitor_state *monitor_info,
              pthread_t *threads);

// func_coder.c
void make_request(struct dongle *left, struct dongle *right,
                  struct coders_state *coder_info);
void delete_requests(struct dongle *left, struct dongle *right,
                     struct coders_state *coder_info);
void check_requests(struct dongle *left, struct dongle *right,
                    struct coders_state *coder_info);
int check_active(struct coders_state *coder_info);
void *coder_func(void *info);

// func_monitor.c
void *finish(struct monitor_state *monitor_info, int error);
void *monitor_func(void *info);

// get_dongle.c
int check_aval(struct dongle *left, struct dongle *right, int coder_id);
long lock_dongle(struct dongle *curr_dongle);
void print_message(struct coders_state *coder_info);
void wait_and_print(long left_wait, long right_wait, struct coders_state *coder_info);
int get_both_dongles(struct dongle *left, struct dongle *right,
                     struct coders_state *coder_info);

// init_coders_data.c
struct thread_vars *init_thread_vars(void);
struct static_data *init_data(int *parsed_args);
int *init_line(void);
struct dongle *init_dongles(int coder_num, struct timespec cooldown);
struct timespec *init_last_compile(int coder_num);

// init_shared_data.c
int *init_coders_active(int coder_num);
pthread_mutex_t *init_output_mutex(void);
struct queue *init_deadline(int coder_num, struct timespec start_burnout);

// init_threads.c
void fill_coders_data1(struct coders_state *coders_info, struct static_data *data,
                       struct thread_vars *sync, struct dongle *dongles,
                       struct timespec *last_compile);
int get_coders_data(int *parsed_args, struct coders_state *coders_info);
void fill_coders_data2(struct coders_state *coder_info, int *coders_active,
                       struct queue *deadline, struct thread_vars *queue,
                       pthread_mutex_t *output_mutex);
int get_shared_data(struct coders_state *coders_info,
                    struct monitor_state *monitor_info);
int init_threads(int *parsed_args, struct coders_state *coders_info,
                 struct monitor_state *monitor_info, pthread_t *threads);

// line_requests.c
void place_fifo_request(int *line, struct coders_state *coder_info);
void place_edf_request(int *line, struct coders_state *coder_info);
void update_line(int *line);
void remove_requests(int *line, int id);
void print_lines(struct dongle *left, struct dongle *right,
                 pthread_mutex_t *output_mutex);

// parsing.c
int *display_error(char *inv_arg, int i);
int *parsing(int argc, char **argv);

// time_conversion.c
struct timespec ms_to_timespec(int time_ms);
long timespec_to_ms(struct timespec time);

// time_utils.c
long get_time_diff(struct timespec time1, struct timespec time2);
struct timespec get_min_time(struct timespec time1, struct timespec time2);
struct timespec add_time(struct timespec time1, struct timespec time2);
struct timespec add_curr_time(struct timespec time);

