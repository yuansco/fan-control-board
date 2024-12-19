
#ifndef __TASK_H__
#define __TASK_H__

#include "board.h"
#include "util.h"

/* should define in board.h if needed */
//#define CONFIG_TASK
//#define CONFIG_TASK_HOOK
//#define CONFIG_TASK_DEBUG
//#define CONFIG_TASK_HOOK_TEST
//#define CONFIG_TASK_HOOK_COMMAND

/******************************************************************************/
/* Task */

/* Task List */
enum task_list {
        TASK_CONSOLE_COMMAND = 0,
#ifdef CONFIG_TASK_HOOK
        TASK_HOOK,
#endif
        TASK_COUNT
};

/* Task polling time(ms) */
enum polling_time {
        TASK_POLLING_CRITICAL = 1,
        TASK_POLLING_5MS      = 5,
        TASK_POLLING_10MS     = 10,
        TASK_POLLING_100MS    = 100,
        TASK_POLLING_SECOND   = 1000,
        TASK_MAX,
};

/* task struct */
struct task {
        /* Task name */
        const char *name;
        /* Task depiction */
        const int task_id;
        /* Task polling time (ms) */
        int pollingtime;
        /* Task wake time (ms) */
        int waketime;
        /* Task function */
        int (*run)(enum task_list task_id);
};

extern struct task task_list[TASK_COUNT];


/******************************************************************************/
/* Hook */

#define HOOK_BUFF_SIZE 16
#define HOOK_DELAY_MAX __INT16_MAX__

enum hook_state {
        HOOK_STATE_INACTIVE = 0,
        HOOK_STATE_SLEEP,
        HOOK_STATE_ACTIVE,
        HOOK_STATE_COUNT
};

enum hook_type {
        HOOK_TYPE_NULL = 0,
        HOOK_TYPE_ONCE,
        HOOK_TYPE_LOOP,
        HOOK_TYPE_COUNT
};

struct hook_data {
        /* Hook state */
        enum hook_state state;
        /* Hook type */
        enum hook_type type;
        /* Hook polling time (ms) */
        int pollingtime;
        /* Hook wake time (ms) */
        int runtime;
        /* Hook processing routine. */
        int (*routine)(void);
};

extern struct hook_data hook_list[HOOK_BUFF_SIZE];


/* Task API */
void task_wake(enum task_list task_id);
void task_update_waketime(enum task_list task_id, int time);
int task_update_pollingtime(enum task_list task_id, enum polling_time time);
void task_print(void);
void task_init(void);
void task_run(void);

/* HOOK API */
#define hook_call_deferred(hook, delay) hook_call(hook, delay, HOOK_TYPE_ONCE)
#define hook_call_loop(hook, delay) hook_call(hook, delay, HOOK_TYPE_LOOP)
int hook_call(int (*hook)(void), int time_delay, enum hook_type type);

void hook_sleep(int hook_id);
void hook_wake(int hook_id);

void hook_print(void);
void hook_init(void);
int hook_task(enum task_list task_id);


#endif /* __TASK_H__ */
