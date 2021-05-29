#include <stdarg.h>
#include <gint/std/stdio.h>
#include <gint/std/stdlib.h>
#include <gint/std/string.h>

#include "log.h"
#include "util.h"
#include "ui.h"

void fxip_log(const char *msg)
{
	ui_write_log(PAGE_LOGS, 0, 0, msg, strlen(msg));
	ui_update_logs();
}

void uip_log(char *msg)
{
	fxip_log(msg);
}

char printf_buffer[128];
void fxip_printf(const char * format, ...)
{
  va_list args;
  va_start (args, format);
  vsnprintf (printf_buffer, sizeof(printf_buffer), format, args);
  fxip_log (printf_buffer);
  va_end (args);
}


void fxip_print_hexdump(const void *addr, uint16_t size)
{
    uint16_t printed = 0;
    uint16_t bufferoffset = 0;
    uint16_t i;

    memset(printf_buffer, 0x00, sizeof(printf_buffer));

    const unsigned char* pc = addr;
    for (i=0; i<size; ++i)
    {
        int  g;
        g = (*(pc+i) >> 4) & 0xf;
        g += g >= 10 ? 'a'-10 : '0';
        printf_buffer[bufferoffset++] = g;
        printed++;

        g = *(pc+i) & 0xf;
        g += g >= 10 ? 'a'-10 : '0';
        printf_buffer[bufferoffset++] = g;
        printed++;
        if (printed % 16 == 0)
        {
          fxip_log (printf_buffer);
          bufferoffset = 0;
          memset(printf_buffer, 0x00, sizeof(printf_buffer));
        }
        else if (printed % 4 == 0) printf_buffer[bufferoffset++] = ' ';
    }
    fxip_log (printf_buffer);
}

void rom_putchar_uart(char asdf)
{
  scif_write(&asdf, 1);
}
void fxip_serial_hexdump(const void *addr, uint16_t size)
{
    uint16_t printed = 0;
    uint16_t i;
    const unsigned char* pc = addr;
    for (i=0; i<size; ++i)
    {
        int  g;
        g = (*(pc+i) >> 4) & 0xf;
        g += g >= 10 ? 'a'-10 : '0';
        rom_putchar_uart(g);
        printed++;
        g = *(pc+i) & 0xf;
        g += g >= 10 ? 'a'-10 : '0';
        rom_putchar_uart(g);
        printed++;
        if (printed % 32 == 0) rom_putchar_uart('\n');
        else if (printed % 4 == 0) rom_putchar_uart(' ');
    }
}
void serial_printf(const char * format, ...)
{
  va_list args;
  va_start (args, format);
  vsnprintf (printf_buffer, sizeof(printf_buffer), format, args);
  scif_write(printf_buffer, strlen(printf_buffer));
  va_end (args);
}
