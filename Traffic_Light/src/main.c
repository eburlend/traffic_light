#include "traffic.h"
#include "hal-api.h"
#include <signal.h>
#include <stdio.h>

static volatile int running = 1;
static void on_sigint(int sig){ (void)sig; running = 0; }

int main(void)
{
    if (hw_init() != 0) {
        fprintf(stderr, "HAL init failed. Try running with sudo.\n");
        return 1;
    }
    signal(SIGINT, on_sigint);  

    traffic_init();

    while (running) {
        if (hw_read_reset_switch()) traffic_init();
        if (hw_read_ped_button())   traffic_request_walk();

        traffic_step();
        hw_delay_ms(T_TICK_MS);
    }

    hw_shutdown();
    return 0;
}
