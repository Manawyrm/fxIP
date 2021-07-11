#pragma once
#include <stdint.h>

void uip_log(char *msg);
void fxip_log(const char *msg);
void myprintf(const char *format, ...);
void fxip_print_hexdump(const void *addr, uint16_t size);
