
#include "task.h"
#include "led.h"
#include "console.h"
#include "main.h"
#include "util.h"

#ifdef CONFIG_TASK

#ifdef CONFIG_TASK_DEBUG
#define CPRINTF(format, args...) PRINTF("TASK: " format, ##args)
#define CPRINTS(format, args...) PRINTS("TASK: " format, ##args)
#else
#define CPRINTF(format, args...)
#define CPRINTS(format, args...)
#endif


void task_wake(enum task_list task_id) {

        task_list[task_id].waketime = 0;
}

void task_update_waketime(enum task_list task_id, int time) {

        task_list[task_id].waketime = HAL_GetTick() + time;
}

int task_update_pollingtime(enum task_list task_id, enum polling_time time) {

        if (time < TASK_POLLING_CRITICAL || time >= TASK_MAX) {
                CPRINTS("\r\n%s fail! id: %d, time:%d",
                                        __func__, task_id, time);
                return EC_ERROR_UNKNOWN;
        }

        task_list[task_id].pollingtime = time;
        return EC_SUCCESS;
}

struct task task_list[] = {
        [TASK_CONSOLE_COMMAND] = {
                .name = "console command task",
                .task_id = TASK_CONSOLE_COMMAND,
                .pollingtime = TASK_POLLING_5MS,
                .waketime = 0,
                .run = &console_command_task,
        },
#ifdef CONFIG_TASK_HOOK
        [TASK_HOOK] = {
                .name = "hook task",
                .task_id = TASK_HOOK,
                .pollingtime = TASK_POLLING_CRITICAL,
                .waketime = 0,
                .run = &hook_task,
        },
#endif
};

void task_print(void) {

        int id;

        CPRINTS("\r\nTask info:");

        for (id = 0; id < ARRAY_SIZE(task_list); id++) {
                CPRINTS(" [Task ID: %d]\r\n    name: %s\r\n    task_id: %d\r\n    pollingtime: %d (ms)",id,
                task_list[id].name, task_list[id].task_id, task_list[id].pollingtime);
        }
}

void task_init(void) {

        int id;

        CPRINTS("init");

        CPRINTS("task list:");

        for (id = 0; id < ARRAY_SIZE(task_list); id++) {

                CPRINTS(" [%d]  %s\r\n      task_id:%d\r\n      pollingtime:%d", id,
                task_list[id].name, task_list[id].task_id, task_list[id].pollingtime);

                // init waketime for first run
                task_update_waketime(id, 0);
        }

#ifdef CONFIG_TASK_HOOK
        hook_init();
#endif /* CONFIG_TASK_HOOK */


}


void task_run(void) {

        int id, rv;
        int time_start, time_spend, time_wake;

        CPRINTS("Task Run!");

        while(1) {

                for (id = 0; id < ARRAY_SIZE(task_list); id++) {

                        rv = EC_SUCCESS;

                        time_start = HAL_GetTick();

                        if(time_start <= task_list[id].waketime)
                                continue;

                        // run task function
                        if (task_list[id].run != NULL)
                                rv = task_list[id].run(id);

                        if (rv != EC_SUCCESS)
                                CPRINTS("[%d] %s return error! (%d)",
                                task_list[id].task_id, task_list[id].name, rv);

                        time_spend = HAL_GetTick() - time_start;

                        // update waketime, waketime = start time + polling time - spend time
                        // don't update waketime if task already update it.
                        if (HAL_GetTick() > task_list[id].waketime) {

                                time_wake = task_list[id].pollingtime - time_spend;
                                task_update_waketime(id, MAX(0, time_wake));
                        }
                }
        }
}

#endif /* CONFIG_TASK */
