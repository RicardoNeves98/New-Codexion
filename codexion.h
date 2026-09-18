#ifndef CODEXION_H
# define CODEXION_H
#endif

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>


// What could really happen: Lets say we have 5 coders and that they make the requests
// all in a row so coder1 if first coder2 is second and so on 
// coder1 get dongle1 and dongle2 and starts compiling 
// coder2 is in line for dongle2 and first for dongle3 
// coder3 is in line for dongle3 and first for dongle4
// coder4 is in line for dongle4 and first for dongle5
// coder5 is in line for both dongle5 and dongle1 
// What happens in this case with the structure we have? 
// What would happen if we has no pull back at all? 1 2 has taken a dongle


typedef struct coders_state
{
    int id;
    int num_compiles;
    int *error;
    int *coders_active;
    struct dongle *left;
    struct dongle *right;
    struct static_data *data;
    struct queue *deadline;
    struct timespec init_limit;
    struct timespec *last_compile;
    struct thread_vars *sync;
    struct thread_vars *queue;
    pthread_mutex_t *output_mutex;
}   coder_state;

typedef struct static_data
{
    int coder_num;
    int time_to_compile;
    int time_to_debug;
    int time_to_refactor;
    int comp_required;
    int cooldown;
    int scheduler;
    struct timespec start_burnout;
    struct timespec comp_burnout;
    struct timespec max_wait;
    struct timespec start_time;
    struct dongle *dongles;
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
    int coder_num;
    struct timespec start_time;
    int *error;
    int *coders_active;
    struct timespec init_limit;
    struct queue *deadline;
    struct thread_vars *queue;
    pthread_mutex_t *output_mutex;
}   monitor_state;

// compile.c
int update_deadline_queue(struct coders_state *coder, int compile);
int update_dongle_state(struct dongle *left, struct dongle *right);
void signal_and_update(struct coders_state *coder, struct timespec now);
int go_work(struct coders_state *coder);

// deadline_queue.c
void move_around(struct queue *deadline, int start, int end);
int get_end_spot(struct queue *deadline, struct timespec burnout, int on_queue,
                 int coder_num, int coder_id, int finished);
void deadline_update(struct queue *deadline, struct timespec burnout, int coder_num,
                     int coder_id, int finished);
void print_deadline(struct queue *deadline, int coder_num);

// free_stuff.c
void free_dongles(struct dongle *dongles, int index, int line);
void free_thread_vars(struct thread_vars *vars);
void free_coders_data(struct coders_state *coder);
void free_monitor_data(struct monitor_state *monitor);
void free_all(struct coders_state *coder, struct monitor_state *monitor,
              pthread_t *threads);

// func_coder.c
void *error_exit(struct thread_vars *queue, int *error);
void make_request(struct dongle *left, struct dongle *right, int coder_id,
                  struct thread_vars *sync, struct timespec *last_compile);
int check_active(struct coders_state *coder);
int get_dongles_and_compile(struct coders_state *coder);
void *coder_func(void *info);

// func_monitor.c
void *error_output(pthread_mutex_t *output_mutex);
void final_output(pthread_mutex_t *output_mutex, struct timespec start,
                  int *coders_active, int burned_coder);
int wait_deadline(struct thread_vars *queue, struct queue *deadline, int *error);
int monitor_coders(struct queue *deadline, struct thread_vars *queue,
                   int *coders_active, int *error);
void *monitor_func(void *info);

// get_dongle.c
int check_aval(struct dongle *left, struct dongle *right, int coder_id);
int wait_and_write(struct timespec left_aval, struct timespec right_wait,
                   struct coders_state *coder);
struct timespec lock_dongle(struct dongle *curr_dongle);
int wait_aval(struct dongle *left, struct dongle *right, int coder_id,
              struct thread_vars *sync, struct timespec max_wait);
int get_dongles(struct coders_state *coder);

// get_structs_data.c
void fill_data(struct coders_state *coders, struct static_data *data,
               struct thread_vars *sync, struct dongle *dongles,
               struct timespec *last_compile);
int get_coders_data(int *parsed_args, struct coders_state *coders);
void share_data(struct coders_state *coder, struct monitor_state *monitor);
int get_shared_data(struct coders_state *coders, struct monitor_state *monitor);
int get_data(int *parsed_args, struct coders_state *coders,
             struct monitor_state *monitor);

// init_coders_data.c
struct thread_vars *init_thread_vars(void);
struct static_data *init_data(int *parsed_args);
int *init_line(void);
struct dongle *init_dongles(int coder_num, struct timespec cooldown);
struct timespec *init_last_compile(int coder_num, int scheduler);

// init_shared_data.c
int *init_error(void);
int *init_coders_active(void);
pthread_mutex_t *init_output_mutex(void);
struct queue *init_deadline(int coder_num);

// init_structs_and_threads.c
struct coders_state *init_coders_struct(int coder_num);
struct monitor_state *init_monitor_struct(void);
void init_threads(struct coders_state *coders, struct monitor_state *monitor,
                  pthread_t *threads);

// line_requests.c
void place_fifo_request(int *line, int coder_id);
void place_edf_request(int *line, int coder_id, struct timespec *last_compile);
void place_request(int *line, int coder_id, struct timespec *last_compile);
void update_line(int *line);
void remove_requests(int *line, int coder_id);
void print_lines(struct dongle *left, struct dongle *right);

// parsing.c
void display_error(char **argv, int inv_ind);
int parse_numbers(char **argv, int *parsed_args);
int parse_scheduler(char *scheduler, int *parsed_args);
int *parsing(int argc, char **argv);

// request_utils.c
void delete_requests(struct dongle *left, struct dongle *right, int coder_id,
                     struct thread_vars *sync);
int check_requests(struct dongle *left, struct dongle *right, int coder_id,
                    struct thread_vars *sync);

// time_utils.c
struct timespec ms_to_timespec(int time_ms);
long timespec_to_ms(struct timespec time);
long get_time_diff(struct timespec time1, struct timespec time2);
long get_min_time(long time1, long time2);
long get_max_time(long time1, long time2);
struct timespec add_time(struct timespec time1, struct timespec time2);

// shared_utils.c
int write_message(struct coders_state *coder, char *action);
int wait_for_threads(struct thread_vars *queue, int *coders_active, int *error,
                     int coder_num, struct timespec init_limit);
int check_all(struct thread_vars *queue, int *coders_active, int *error,
              int coder_num, struct timespec init_limit);

