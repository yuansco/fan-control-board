
#include "main.h"
#include "task.h"
#include "console.h"
#include "util.h"
#include "linklist.h"

/******************************************************************************/
/* Hook */

#if defined(CONFIG_TASK_HOOK) && defined(CONFIG_TASK_HOOK_DYNAMIC)

#ifdef CONFIG_TASK_DEBUG
#define CPRINTF(format, args...) PRINTF("HOOK: " format, ##args)
#define CPRINTS(format, args...) PRINTS("HOOK: " format, ##args)
#else
#define CPRINTF(format, args...)
#define CPRINTS(format, args...)
#endif

static int hook_debug_level;

// Global variable: pointer to the head of the hook linked list
static ListNode *hook_list_head = NULL;

void hook_debug(int debug_level) {

        hook_debug_level = !!debug_level;
}

// Helper: used to free the memory of hook_data (passed to list_remove/list_destroy)
static void free_hook_data(void *data) {
        if (data)
                free(data);
}

// hook_reset() now receives a ListNode pointer and removes it from the list
// Note: Once reset, this hook_node pointer becomes invalid
static void hook_reset(ListNode *hook_node) {

        if (!hook_node)
                return;

        // Remove the node from the list and free its contained hook_data
        list_remove(&hook_list_head, hook_node, free_hook_data);
        // Debug information
        if (hook_debug_level)
                CPRINTS("removed node %p", hook_node);
}

// Modified to receive a ListNode pointer
void hook_sleep(ListNode *hook_node) {

        if (!hook_node || !hook_node->data)
                return;

        struct hook_data *data = (struct hook_data *)hook_node->data;
        data->state = HOOK_STATE_SLEEP;
}

// Modified to receive a ListNode pointer
void hook_wake(ListNode *hook_node) {

        if (!hook_node || !hook_node->data)
                return;

        struct hook_data *data = (struct hook_data *)hook_node->data;

        if (data->state != HOOK_STATE_SLEEP)
                return;

        data->state = HOOK_STATE_ACTIVE;
        // Or set to HAL_GetTick() as needed
        data->runtime = 0;
}

// update hook runtime
static int hook_update_runtime(ListNode *hook_node, int time_start) {

        struct hook_data *data = (struct hook_data *)hook_node->data;
        int should_remove = 0; // Flag to indicate if the node should be removed

        if (!hook_node || !hook_node->data)
                // Return 0 to indicate node should not be removed due to error
                return 0;

        switch (data->type) {
        case HOOK_TYPE_ONCE:
                // hook_reset(hook_node); // Do not directly call reset here, but return a flag
                should_remove = 1;
                break;
        case HOOK_TYPE_LOOP:
                data->runtime = time_start + data->pollingtime;
                break;
        default:
                CPRINTS("Unknown hook type! Removing hook.");
                // hook_reset(hook_node); // Do not directly call reset here, but return a flag
                should_remove = 1;
                break;
        }
        return should_remove;
}

// hook_call() now returns a ListNode pointer as the hook's handle/ID
// Returns NULL on failure
int hook_call(int (*hook_func)(void), int time_delay, enum hook_type type) {

        int time_run;
        int time_polling = 0;

        if (time_delay < 0 || time_delay >= HOOK_DELAY_MAX) {
                CPRINTS(" %s fail! Invalid time_delay:%d", __func__, time_delay);
                return -1;
        }
        if (!hook_func) {
                CPRINTS(" %s fail! hook_func is NULL", __func__);
                return -1;
        }

        // 1. Allocate memory for hook_data
        struct hook_data *new_hook_data = (struct hook_data *)malloc(sizeof(struct hook_data));

        if (!new_hook_data) {
                CPRINTS("Failed to allocate memory for hook data!");
                return -1; // Memory allocation failed
        }

        // 2. Fill in hook_data
        time_run = HAL_GetTick() + time_delay;

        if (type == HOOK_TYPE_LOOP)
                time_polling = time_delay;

        new_hook_data->state = HOOK_STATE_ACTIVE;
        new_hook_data->type = type;
        new_hook_data->pollingtime = time_polling;
        new_hook_data->runtime = time_run;
        new_hook_data->routine = hook_func;

        // 3. Add hook_data to the linked list
        ListNode *new_node = list_add(&hook_list_head, new_hook_data);

        if (!new_node) {
                CPRINTS("Failed to add hook data to list!");
                // Free allocated data memory if adding to the list fails
                free(new_hook_data);
                // Addition failed
                return -1; 
        }

        // Debug information
        if (hook_debug_level)
                CPRINTS("added node %p", new_node);

        task_wake(TASK_HOOK);

        // Return the newly created node pointer as ID/handle
        return (int) new_node; 
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

        ListNode *current = hook_list_head;
        // Used to simulate the old index output
        int print_index = 0;

        CPRINTS("\r\nHook Info :");

        // Return directly if the list is empty
        if (current == NULL) {
                CPRINTS(" (No active hooks)");
                return; 
        }

        while (current != NULL) {

                // Check if the data pointer is valid
                if (current->data) { 
                        struct hook_data *data = (struct hook_data *)current->data;

                // --- Safely retrieve state and type names ---
                const char *state_str = "UnknownState";

                // Perform bounds checking (assuming the enum starts from 0)
                // HOOK_STATE_COUNT is the size of the enum
                if (data->state >= 0 && data->state < HOOK_STATE_COUNT) {
                        state_str = hook_state_name[data->state];
                }

                const char *type_str = "UnknownType";
                        // Perform bounds checking
                        // HOOK_TYPE_COUNT is the size of the enum
                        if (data->type >= 0 && data->type < HOOK_TYPE_COUNT) {
                                type_str = hook_type_name[data->type];
                        }
                // --- End of safe retrieval ---

                // print information, format similar to the old version
                // also print the node address (current), as this is the true unique identifier
                CPRINTS("\r\n [%d] Node:%p State:%s, Type:%s, Polling:%dms, Run:%dms",
                        print_index,         // Print simulated index
                        (void*)current,      // Print node memory address (actual handle)
                        state_str,           // State name
                        type_str,            // Type name
                        data->pollingtime,   // Polling time
                        data->runtime);      // Next run time
                } else {
                        // Handle the abnormal case where the data pointer is NULL
                        CPRINTS("\r\n [%d] Node:%p Error: Hook data is NULL!",
                                                        print_index, (void*)current);
                }

                // Increment the print index
                print_index++;
                // Move to the next node
                current = current->next;
        }

        // Add an end marker
        CPRINTS("\r\n--- End Hook Info ---");
}


#endif

/******************************************************************************/
/* Hook task */

void hook_init(void) {

        CPRINTS("init");

#ifdef CONFIG_TASK_HOOK_TEST
        hook_test();
#endif
}

// hook_task() will iterates through the linked list
int hook_task(enum task_list task_id) {
        int time_start;
        int rv;
        ListNode *current = hook_list_head;
        ListNode *next_node = NULL;

        while (current != NULL) {
                // Important: Get next before processing current,
                // as current might be removed during processing
                next_node = current->next;

                if (!current->data) {
                        CPRINTS("Error: Hook node %p has NULL data!", current);
                        // Consider whether to remove this corrupted node
                        // Assuming no data to free as it's NULL
                        // list_remove(&hook_list_head, current, NULL);
                        current = next_node;
                        continue;
                }

                struct hook_data *data = (struct hook_data *)current->data;

                // Skip hooks that are not active or in sleep state
                // HOOK_STATE_INACTIVE no longer exists, removal implies inactive
                if (data->state == HOOK_STATE_SLEEP) {
                        current = next_node;
                        continue;
                }

                time_start = HAL_GetTick();

                // Check if the execution time has arrived
                if (time_start < data->runtime) {
                        current = next_node;
                        continue;
                }

                // Execute the hook routine
                rv = data->routine();

                if (rv) {
                        CPRINTS("Hook node %p routine returned error! (%d)", current, rv);
                        // Can decide whether to remove the errored hook as needed
                        // hook_reset(current); // If removal is needed on error
                }

                // Update runtime or decide whether to remove the hook (e.g., ONCE type)
                int should_remove = hook_update_runtime(current, time_start);
                if (should_remove) {
                        hook_reset(current); // Remove from the list and free resources
                        // The current pointer is now invalid, no need to move to next_node
                        // (as reset handles it). However, for the next iteration of the
                        // loop, we need current to point to next_node.
                        // Since hook_reset internally handles list connections,
                        // directly assigning next_node here is sufficient
                }
                // else {
                //         If the node was not removed, move to the next node
                //         current = next_node; // This line is now at the beginning of the loop
                // }
                current = next_node; // Move to the next node for processing
        }
        return EC_SUCCESS;
}

#endif /* defined(CONFIG_TASK_HOOK) && defined(CONFIG_TASK_HOOK_DYNAMIC) */