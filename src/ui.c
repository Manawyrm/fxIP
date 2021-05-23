#include "ui.h"
#include <gint/display.h>
#include <gint/keyboard.h>
#include <stdarg.h>
#include <gint/std/stdio.h>
#include <gint/std/stdlib.h>
#include <gint/std/string.h>

#include "util.h"
#include "log.h"

#include "uip/uip.h"

extern struct uip_stats uip_stat;

typedef enum { 
	PAGE_LOGS,
	PAGE_STATS,
	PAGE_IRC
} page_t;

page_t current_page = PAGE_LOGS;

int ui_handle_keyboard()
{
	// keyboard input
	key_event_t event = pollevent();
	if (event.type == KEYEV_UP)
	{
		if (event.key == KEY_F1)
		{
			current_page = PAGE_LOGS;
			ui_update();
		}
		if (event.key == KEY_F2)
		{
			current_page = PAGE_STATS;
			ui_update();
		}
		if (event.key == KEY_F3)
		{
			current_page = PAGE_IRC;
			ui_update();
		}
		if (event.key == KEY_EXIT)
		{
			return -1;
		}
	}
	return 0;
}

void ui_update_logs()
{
	if (current_page == PAGE_LOGS)
	{
		ui_update();
	}
}

void ui_update()
{
	dclear(C_WHITE);

	switch (current_page)
	{
	case PAGE_LOGS: 
		for (unsigned int i = 0; i < ARRAY_SIZE(display_scroll_buf); i++)
		{
			unsigned int line_idx = (log_idx + i) % ARRAY_SIZE(display_scroll_buf);

			dtext(1, i * 10, C_BLACK, display_scroll_buf[line_idx]);
		}
		break;
	case PAGE_STATS: 
		ui_printf(0, 0, C_BLACK, "IP RX: %u TX: %u", uip_stat.ip.recv, uip_stat.ip.sent);
		ui_printf(20, 10, C_BLACK, "Dropped: %u", uip_stat.ip.drop);
		ui_printf(0, 23, C_BLACK, "ICMP RX: %u TX: %u", uip_stat.icmp.recv, uip_stat.icmp.sent);

		ui_printf(1, 35, C_BLACK, "TCP:");
		ui_printf(10, 45, C_BLACK, "Chksum err: %u", uip_stat.tcp.chkerr);
		ui_printf(10, 55, C_BLACK, "Rexmit: %u", uip_stat.tcp.rexmit);

		break;
	default: 
		dtext(1, 10, C_BLACK, "Invalid page.");
	}
	
	dupdate();
}

extern char printf_buffer[128];
void ui_printf(int x, int y, int fg, const char * format, ...)
{
	va_list args;
	va_start (args, format);
	vsnprintf (printf_buffer, sizeof(printf_buffer), format, args);
	dtext(x, y, fg, printf_buffer);
	va_end (args);
}
