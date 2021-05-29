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

uint8_t cursor_blink = 0;

char page_logs_input_buffer[128];
uint8_t objectlog_storage[1024];
objectlog_t objectlog;

void ui_cursor_blink()
{
	cursor_blink = !cursor_blink;
}

void ui_page_logs_submit(page_t *page)
{
	fxip_log(page->input_buffer);
}

const page_t pages[] = {
	{
		/* Logs */
		.id	= PAGE_LOGS,
		.key	= KEY_F1,
		.render_callback = ui_render_logs,
		.input_enabled = 1,
		.input_buffer = page_logs_input_buffer,
		.input_buffer_size = sizeof(page_logs_input_buffer),
		.input_submit_callback = ui_page_logs_submit
	},
	{
		/* Statistics */
		.id	= PAGE_STATISTICS,
		.key	= KEY_F2,
		.render_callback = ui_render_statistics,
		.input_enabled = 0
	},
	{
		/* IRC */
		.id	= PAGE_IRC,
		.key	= KEY_F3,
		.render_callback = ui_render_irc,
		.input_enabled = 0
	},
};

page_t *current_page = &pages[0];


void ui_update()
{
	dclear(C_WHITE);
	current_page->render_callback(current_page);
	if (current_page->input_enabled)
	{
		drect(0, 55, 128, 64, C_BLACK);

		char *scrolled_input = current_page->input_buffer;
		if (current_page->input_offset > UI_DISPLAY_CHARS)
		{
			scrolled_input += MIN(current_page->input_scroll_offset,
				current_page->input_offset - UI_DISPLAY_CHARS);
		}
		uint16_t scrolled_input_length = strlen(scrolled_input);

		dtext_opt(1, 56, C_WHITE, C_NONE, DTEXT_LEFT, DTEXT_TOP, scrolled_input, scrolled_input_length);

		if (cursor_blink)
		{
			int width = 0;
			int height = 0;

			dnsize(scrolled_input, scrolled_input_length, NULL, &width, &height);

			dtext_opt(1 + width, 56, C_WHITE, C_NONE, DTEXT_LEFT, DTEXT_TOP, "_", 1);
		}

	}

	dupdate();
}

void ui_update_logs()
{
	if (current_page->id == 0)
	{
		ui_update();
	}
}

void ui_render_logs(page_t *page)
{
	for (unsigned int i = 0; i < objectlog.num_entries && i < UI_DISPLAY_LINES ; i++)
	{
		uint8_t pencil_x = 0;
		uint8_t pencil_y = (UI_DISPLAY_LINES - i) * 8;
		int width = 0;
		int height = 0;

		message_hdr_t hdr;
		uint8_t length;
		objectlog_iterator_t iter;
		const uint8_t *ptr;
		ptr = ui_objectlog_get_message(&objectlog, -i, &iter, &hdr, &length);
		if (!ptr)
		{
			continue;
		}

		if (hdr.page_id == PAGE_LOGS)
		{
			do {
				if (length)
				{
					dtext_opt(pencil_x, pencil_y, C_BLACK, C_NONE, DTEXT_LEFT, DTEXT_TOP, ptr, length);

					dnsize(ptr, length, NULL, &width, &height);
					pencil_x += width;
					if (pencil_x > UI_DISPLAY_PIXEL_X)
					{
						goto next;
					}
				}

				iter = objectlog_next(&objectlog, iter);
				ptr = objectlog_get_fragment(&objectlog, iter, &length);
			} while (iter >= 0);
		}

next:
	}
}

const void *ui_objectlog_get_message(objectlog_t *log, int object_idx, objectlog_iterator_t *riter, message_hdr_t *hdr, uint8_t *len)
{
	uint8_t length;
	uint8_t bytes_read = 0;
	const uint8_t *ptr;
	objectlog_iterator_t iter = objectlog_iterator(log, object_idx);

	if (iter < 0)
	{
		return NULL;
	}

	do {
		ptr = objectlog_get_fragment(log, iter, &length);

		uint8_t cpylen = MIN(sizeof(message_hdr_t) - bytes_read, length);
		memcpy(((uint8_t*)hdr) + bytes_read, ptr, cpylen);
		bytes_read += cpylen;
	} while (bytes_read < sizeof(message_hdr_t));

	*riter = iter;
	*len = length - bytes_read;
	return ptr + bytes_read;
}

void ui_init()
{
	for (unsigned int i = 0; i < ARRAY_SIZE(pages); i++)
	{
		if (pages[i].input_buffer)
		{
			memset(pages[i].input_buffer, 0x00, pages[i].input_buffer_size);
		}
	}

	objectlog_init(&objectlog, objectlog_storage, sizeof(objectlog_storage));
}

void ui_write_log(uint8_t page_id, uint8_t type, uint8_t channel, char *data, uint16_t length)
{
	message_hdr_t hdr;
	hdr.page_id = page_id;
	hdr.type = type;
	hdr.channel = channel;

	scatter_object_t scatter_list[] = {
		{ .ptr = &hdr, .len = sizeof(hdr) },
		{ .ptr = data, .len = length },
		{ .len = 0 }
	};

	objectlog_write_scattered_object(&objectlog, scatter_list);
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

