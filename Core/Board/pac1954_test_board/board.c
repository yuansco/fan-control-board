#include <stdint.h>

#include "board.h"
#include "console.h"
#include "main.h"
#include "pac1954.h"
#include "task.h"
#include "util.h"

#define PAC1954_POLLING_TIME_MS 5000

int pac_print_loop(void) {

        pac1954_print();
        return EC_SUCCESS;
}

/* override board_init function */
__override void board_init(void) {
        hook_call_loop(&pac_print_loop, PAC1954_POLLING_TIME_MS);
}
