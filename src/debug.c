#include "debug.h"
void rom_putchar_uart(uint8_t c)
{
  scif_write(&c, 1);
}

void print_memory(const void *addr, uint16_t size)
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


char buffer[128];
void fxip_printf( const char * format, ... )
{
  va_list args;
  va_start (args, format);
  vsnprintf (buffer, 128, format, args);
  fxip_log (buffer);
  va_end (args);
}
