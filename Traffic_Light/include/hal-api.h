#ifndef HAL_API_H
#define HAL_API_H
#include <stdint.h>

int  hw_init(void);              // returns 0 on success
void hw_shutdown(void);          // unmap / close
int  hw_read_reset_switch(void); // KEY0
int  hw_read_ped_button(void);   // KEY1

void   hw_leds_write(uint32_t value);   // write LEDR
void   hw_hex40_write(uint32_t pattern);// HEX3..HEX0 
void   hw_hex54_write(uint32_t pattern);// HEX5..HEX4 

uint8_t hw_seg7_encode_digit(int digit); // 0..9 -> segments 
uint8_t hw_seg7_encode_char(char c);     // limited letters for "Pr", "On", "-"

void hw_delay_ms(int ms);                /

// LED mapping for traffic
#define LED_GREEN   (1u<<0)  // LEDR0
#define LED_YELLOW  (1u<<1)  // LEDR1
#define LED_RED     (1u<<2)  // LEDR2

#define T_TICK_MS 100

#endif 
