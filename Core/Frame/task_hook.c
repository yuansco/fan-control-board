
#include "main.h"
#include "task.h"
#include "console.h"
#include "util.h"

/******************************************************************************/
/* Hook */

#ifdef CONFIG_TASK_HOOK

#ifdef CONFIG_TASK_DEBUG
#define CPRINTF(format, args...) PRINTF("HOOK: " format, ##args)
#define CPRINTS(format, args...) PRINTS("HOOK: " format, ##args)
#else
#define CPRINTF(format, args...)
#define CPRINTS(format, args...)
#endif


struct hook_data hook_list[HOOK_BUFF_SIZE];

static int hook_find_next(int *index) {

        int i;

        for (i = 0; i < ARRAY_SIZE(hook_list); i++) {
                if (hook_list[i].state == HOOK_STATE_INACTIVE)
                        break;
        }

        if (i >= ARRAY_SIZE(hook_list)) {
                CPRINTS("hook overflow!");
                return EC_ERROR_UNKNOWN;
        }

        *index = i;

        return EC_SUCCESS;
}

static void hook_reset(int hook_id) {

        struct hook_data data = {
                .state = HOOK_STATE_INACTIVE,
                .type = HOOK_TYPE_NULL,
                .pollingtime = -1,
                .runtime = -1,
                .routine = NULL
        };

        hook_list[hook_id] = data;
}

void hook_sleep(int hook_id) {

        hook_list[hook_id].state = HOOK_STATE_SLEEP;
}

void hook_wake(int hook_id) {

        if (hook_list[hook_id].state != HOOK_STATE_SLEEP)
                return;
        
        hook_list[hook_id].state = HOOK_STATE_ACTIVE;
        hook_list[hook_id].runtime = 0;
}


static void hook_update_runtime(int hook_id, int time_start) {

        switch (hook_list[hook_id].type) {
        case HOOK_TYPE_ONCE:
                hook_reset(hook_id);
                break;
        case HOOK_TYPE_LOOP:
                hook_list[hook_id].runtime = time_start + hook_list[hook_id].pollingtime;
                break;
        default:
                CPRINTS("Unknown hook type!");
                hook_reset(hook_id);
                break;
        }
}

int hook_call(int (*hook)(void), int time_delay, enum hook_type type) {

        int time_run, id, time_polling = 0;

        if (time_delay < 0 || time_delay >= HOOK_DELAY_MAX) {
                CPRINTS("\r\n%s fail! time:%d", __func__, time_delay);
                return EC_ERROR_UNKNOWN;
        }

        time_run = HAL_GetTick() + time_delay;

        if (type == HOOK_TYPE_LOOP)
                time_polling = time_delay;

        struct hook_data data = {
                .state = HOOK_STATE_ACTIVE,
                .type = type,
                .pollingtime = time_polling,
                .runtime = time_run,
                .routine = hook,
        };

        if (hook_find_next(&id) == EC_ERROR_UNKNOWN)
                return EC_COMMAND_NOT_FOUND;

        hook_list[id] = data;
        task_wake(TASK_HOOK);
        return id;
}

/******************************************************************************/
/* Hook test */

#ifdef CONFIG_TASK_HOOK_TEST

static int start_time;

static int hook_deferred_print(void) {

        int run_time = HAL_GetTick();

        CPRINTS("%s (%d)", __func__, (run_time - start_time));

        return EC_SUCCESS;
}

static void hook_test(void) {

        CPRINTS("%s", __func__);

        start_time = HAL_GetTick();

        hook_call_deferred(&hook_deferred_print, 3000);
        hook_call_deferred(&hook_deferred_print, 4000);
        hook_call_deferred(&hook_deferred_print, 5000);
}
#endif

/******************************************************************************/
/* Hook console command */

#ifdef CONFIG_TASK_HOOK_COMMAND

static const char *const hook_state_name[] = {
        [HOOK_STATE_INACTIVE] = "HOOK_STATE_INACTIVE",
        [HOOK_STATE_SLEEP] = "HOOK_STATE_SLEEP",
        [HOOK_STATE_ACTIVE] = "HOOK_STATE_ACTIVE",
};

static const char *const hook_type_name[] = {
        [HOOK_TYPE_NULL] = "HOOK_TYPE_NULL",
        [HOOK_TYPE_ONCE] = "HOOK_TYPE_ONCE",
        [HOOK_TYPE_LOOP] = "HOOK_TYPE_LOOP",
};

void hook_print(void) {

        int id;

        CPRINTS("\r\nHook info:");

        for (id = 0; id < ARRAY_SIZE(hook_list); id++) {
                CPRINTS(" [%d] state:%s, type:%s, polling:%d, run:%d", id,
                        hook_state_name[hook_list[id].state],
                        hook_type_name[hook_list[id].type],
                        hook_list[id].pollingtime,
                        hook_list[id].runtime);
        }
}

#endif

/******************************************************************************/
/* Hook task */

void hook_init(void) {

        CPRINTS("init");

        for (int i = 0; i < ARRAY_SIZE(hook_list); i++)
                hook_reset(i);

#ifdef CONFIG_TASK_HOOK_TEST
        hook_test();
#endif
}

int hook_task(enum task_list task_id) {

        int time_start;
        int rv;

        for (int id = 0; id < ARRAY_SIZE(hook_list); id++) {

                if(hook_list[id].state == HOOK_STATE_INACTIVE ||
                   hook_list[id].state == HOOK_STATE_SLEEP)
                        continue;

                time_start = HAL_GetTick();

                if (time_start < hook_list[id].runtime)
                        continue;

                rv = hook_list[id].routine();

                if (rv)
                        CPRINTS("hook return fail! (%d)", rv);

                hook_update_runtime(id, time_start);
        }

        return EC_SUCCESS;
}

#endif /* CONFIG_TASK_HOOK */