#pragma once
#include <stdint.h>

void uip_log(char *msg);
void fxip_log(const char *msg);
void myprintf(const char *format, ...);
void fxip_print_hexdump(const void *addr, uint16_t size);

typedef char log_msg_t[32];
extern log_msg_t display_scroll_buf[6];
extern unsigned int log_idx;
