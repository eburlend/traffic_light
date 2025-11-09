#ifndef ADDRESS_MAP_ARM_H
#define ADDRESS_MAP_ARM_H

#define LW_BRIDGE_BASE      0xFF200000
#define LW_BRIDGE_SPAN      0x00005000

#define LEDR_BASE           0x00000000
#define HEX3_HEX0_BASE      0x00000020
#define HEX5_HEX4_BASE      0x00000030
#define SW_BASE             0x00000040
#define KEY_BASE            0x00000050

#define PIO_DATA            0x0
#define PIO_DIR             0x4
#define PIO_INTMSK          0x8
#define PIO_EDGE            0xC

#endif 
