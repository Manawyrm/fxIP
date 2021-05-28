#pragma once
#include <stdint.h>

#define UI_DISPLAY_PIXEL_X 128
#define UI_DISPLAY_PIXEL_Y 64
#define UI_DISPLAY_LINES 6
#define UI_DISPLAY_CHARS 21

int ui_handle_keyboard();
void ui_update();
void ui_update_logs();
void ui_init();

void ui_printf(int x, int y, int fg, const char * format, ...);

struct page;

struct page
{
	uint8_t id;
	uint8_t key;
	void (*render_callback)(struct page *page);
	uint8_t input_enabled;
	char *input_buffer;
	uint16_t input_buffer_size;
	uint16_t input_offset;
	uint16_t input_scroll_offset;
	void (*input_submit_callback)(struct page *page);
};

typedef struct page page_t;

void ui_render_logs(page_t *page);
void ui_render_statistics(page_t *page);
void ui_render_irc(page_t *page);

extern page_t *current_page;
extern const page_t pages[3];