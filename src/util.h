#pragma once

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))

#define BIT(x) (1 << (x))
#define BITMASK(end, start) (((BIT(end + 1) >> start) - 1) << start)

#define MMIO8(addr) (*(volatile uint8_t*)(addr)) 
#define MMIO16(addr) (*(volatile uint16_t*)(addr)) 
