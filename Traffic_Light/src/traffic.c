#include "traffic.h"
#include "hal-api.h"
#include <stdint.h>

typedef enum { GREEN=0, YELLOW=1, RED=2 } ns_light_t;

static struct {
    ns_light_t ns;
    int t_ms;            // phase timer
    int ped_req;         // queued request
    int walk_active;     // 1 while honoring request 
    int walk_ms;         // simple walk duration
} ctx;

// durations
#define T_GREEN_MS   3000
#define T_YELLOW_MS  1000
#define T_RED_MS     3000
#define T_WALK_MS    2000   // simple fixed walk window during RED

static void drive_leds(void) {
    uint32_t leds = 0;
    if (ctx.ns == GREEN)  leds |= LED_GREEN;
    if (ctx.ns == YELLOW) leds |= LED_YELLOW;
    if (ctx.ns == RED)    leds |= LED_RED;
    hw_leds_write(leds);
}

static void drive_hex(void) {
    uint8_t dash = hw_seg7_encode_char('-');                  // segment encoding for '-'

    if (ctx.walk_active) {                                    // if pedestrian walk is in progress
        uint8_t O = hw_seg7_encode_char('O');
        uint8_t N = hw_seg7_encode_char('N');
        uint32_t low = ((~N & 0x7F)<<0) | ((~O & 0x7F)<<8) | (0xFFu<<16) | (0xFFu<<24);
        hw_hex40_write(low);                                  // show "ON"
    } else if (ctx.ped_req) {                                 // pedestrian request queued
        uint8_t P = hw_seg7_encode_char('P');
        uint8_t R = hw_seg7_encode_char('R');
        uint32_t low = ((~R & 0x7F)<<0) | ((~P & 0x7F)<<8) | (0xFFu<<16) | (0xFFu<<24);
        hw_hex40_write(low);                                  // show "PR"
    } else {                                                  // default display: no request
        uint32_t low = ((~dash & 0x7F)<<0) | ((~dash & 0x7F)<<8) | (0xFFu<<16) | (0xFFu<<24);
        hw_hex40_write(low);                                  // show "--"
    }

    hw_hex54_write(0xFFFFFFFFu);                              // turn off HEX5 & HEX4
}


void traffic_init(void) {
    ctx.ns = GREEN;
    ctx.t_ms = T_GREEN_MS;
    ctx.ped_req = 0;
    ctx.walk_active = 0;
    ctx.walk_ms = 0;
    drive_leds();
    drive_hex();
}

void traffic_request_walk(void) {
    ctx.ped_req = 1;
}

void traffic_step(void) {
    ctx.t_ms -= T_TICK_MS;

    if (ctx.walk_active) {
        ctx.walk_ms -= T_TICK_MS;
        if (ctx.walk_ms <= 0) {
            ctx.walk_active = 0; 
        }
    }

    switch (ctx.ns) {
    case GREEN:
        if (ctx.t_ms <= 0) { ctx.ns = YELLOW; ctx.t_ms = T_YELLOW_MS; }
        break;

    case YELLOW:
        if (ctx.t_ms <= 0) { ctx.ns = RED; ctx.t_ms = T_RED_MS; }
        break;

    case RED:
        if (ctx.ped_req && !ctx.walk_active) {
            ctx.walk_active = 1;
            ctx.walk_ms = T_WALK_MS;
            ctx.ped_req = 0;
        }
        if (ctx.t_ms <= 0) { ctx.ns = GREEN; ctx.t_ms = T_GREEN_MS; }
        break;
    }

    drive_leds();
    drive_hex();
}
