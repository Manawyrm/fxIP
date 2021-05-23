#include "ui.h"
#include <gint/display.h>
#include <gint/keyboard.h>
#include "util.h"
#include "log.h"

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
		dtext(1, 10, C_BLACK, "Statistik! :D");
		break;
	default: 
		dtext(1, 10, C_BLACK, "Invalid page.");
	}
	
	dupdate();
}

