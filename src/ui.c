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

const page_t pages[] = {
	{
		/* Logs */
		.id	= 0,
		.key	= KEY_F1,
		.render_callback = ui_render_logs,
	},
	{
		/* Statistics */
		.id	= 1,
		.key	= KEY_F2,
		.render_callback = ui_render_statistics,
	},
	{
		/* IRC */
		.id	= 2,
		.key	= KEY_F3,
		.render_callback = ui_render_irc,
	},
};

page_t current_page = pages[0];

void ui_update_logs()
{
	if (current_page.id == 0)
	{
		ui_update();
	}
}

void ui_update()
{
	dclear(C_WHITE);
	current_page.render_callback(&current_page);
	dupdate();
}

void ui_render_logs(page_t *page)
{
	for (unsigned int i = 0; i < ARRAY_SIZE(display_scroll_buf); i++)
	{
		unsigned int line_idx = (log_idx + i) % ARRAY_SIZE(display_scroll_buf);

		dtext(1, i * 10, C_BLACK, display_scroll_buf[line_idx]);
	}
}

void ui_render_statistics(page_t *page)
{
	ui_printf(0, 0, C_BLACK, "IP RX: %u TX: %u", uip_stat.ip.recv, uip_stat.ip.sent);
	ui_printf(20, 10, C_BLACK, "Dropped: %u", uip_stat.ip.drop);
	ui_printf(0, 23, C_BLACK, "ICMP RX: %u TX: %u", uip_stat.icmp.recv, uip_stat.icmp.sent);

	ui_printf(1, 35, C_BLACK, "TCP:");
	ui_printf(10, 45, C_BLACK, "Chksum err: %u", uip_stat.tcp.chkerr);
	ui_printf(10, 55, C_BLACK, "Rexmit: %u", uip_stat.tcp.rexmit);
}

void ui_render_irc(page_t *page)
{
	dtext(1, 10, C_BLACK, "Invalid page.");
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

