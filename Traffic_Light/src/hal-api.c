#define _GNU_SOURCE
#include "hal-api.h"
#include "address_map_arm.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

static int mem_fd = -1;
static volatile uint8_t * lw = 0;

static inline volatile uint32_t* pio_ptr(uint32_t base_off, uint32_t reg_off) {
    return (volatile uint32_t*)(lw + base_off + reg_off);
}

int hw_init(void) {
    mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mem_fd < 0) { perror("open(/dev/mem)"); return -1; }

    lw = (uint8_t*) mmap(NULL, LW_BRIDGE_SPAN, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, LW_BRIDGE_BASE);
    if (lw == MAP_FAILED) { perror("mmap LW"); lw = 0; close(mem_fd); mem_fd = -1; return -2; }

    // Set outputs for LEDs/HEX, inputs for KEY
    *pio_ptr(LEDR_BASE,     PIO_DIR) = 0x3FF;       // 10 LEDs
    *pio_ptr(HEX3_HEX0_BASE,PIO_DIR) = 0xFFFFFFFF;
    *pio_ptr(HEX5_HEX4_BASE,PIO_DIR) = 0xFFFFFFFF;
    *pio_ptr(KEY_BASE,      PIO_DIR) = 0x00000000;  // inputs

    // Clear outputs 
    *pio_ptr(LEDR_BASE,     PIO_DATA) = 0;
    *pio_ptr(HEX3_HEX0_BASE,PIO_DATA) = 0xFFFFFFFF;
    *pio_ptr(HEX5_HEX4_BASE,PIO_DATA) = 0xFFFFFFFF;
    return 0;
}

void hw_shutdown(void) {
    if (lw && lw != MAP_FAILED) { munmap((void*)lw, LW_BRIDGE_SPAN); lw = 0; }
    if (mem_fd >= 0) { close(mem_fd); mem_fd = -1; }
}

int hw_read_reset_switch(void) {
    if (!lw) return 0;
    uint32_t v = *pio_ptr(KEY_BASE, PIO_DATA);
    return (v & 0x1) ? 1 : 0;  // KEY0
}

int hw_read_ped_button(void) {
    if (!lw) return 0;
    uint32_t v = *pio_ptr(KEY_BASE, PIO_DATA);
    return (v & 0x2) ? 1 : 0;  // KEY1
}

void hw_leds_write(uint32_t value) {
    if (!lw) return;
    *pio_ptr(LEDR_BASE, PIO_DATA) = value & 0x3FF;
}

// 7-seg maps 
static const uint8_t seg_map_digit[10] = {
    0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F // 0..9
};

// Minimal letters: '-', 'P', 'R', 'O', 'N' 
static uint8_t seg_map_char(char c) {
    switch (toupper((unsigned char)c)) {
        case '-': return 0x40;     // minus 
        case 'P': return 0x73;     // P 
        case 'R': return 0x50;     // R 
        case 'O': return 0x3F;     // same as 0
        case 'N': return 0x54;     // N 
        default:  return 0x00;     // blank
    }
}

uint8_t hw_seg7_encode_digit(int digit) {
    if (digit < 0 || digit > 9) return 0;
    return seg_map_digit[digit];
}

uint8_t hw_seg7_encode_char(char c) { return seg_map_char(c); }

void hw_hex40_write(uint32_t pattern) { if (lw) *pio_ptr(HEX3_HEX0_BASE, PIO_DATA)=pattern; }
void hw_hex54_write(uint32_t pattern) { if (lw) *pio_ptr(HEX5_HEX4_BASE, PIO_DATA)=pattern; }

void hw_delay_ms(int ms){ usleep(ms*1000); }
