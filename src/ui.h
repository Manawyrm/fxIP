#pragma once
#include <stdint.h>

int ui_handle_keyboard();
void ui_update();
void ui_update_logs();

void ui_printf(int x, int y, int fg, const char * format, ...);

struct page;

struct page
{
	uint8_t id;
	uint8_t key;
	void (*render_callback)(struct page *page);
};

typedef struct page page_t;

void ui_render_logs(page_t *page);
void ui_render_statistics(page_t *page);
void ui_render_irc(page_t *page);

extern page_t current_page;
extern const page_t pages[3];