#pragma once
#include <stdint.h>
#include "objectlog.h"

#define UI_DISPLAY_PIXEL_X 128
#define UI_DISPLAY_PIXEL_Y 64
#define UI_DISPLAY_LINES 7
#define UI_DISPLAY_CHARS 21

int ui_handle_keyboard();
void ui_update();
void ui_update_logs();
void ui_init();

void ui_printf(int x, int y, int fg, const char * format, ...);

typedef enum {
	PAGE_LOGS,
	PAGE_STATISTICS,
	PAGE_IRC_OVERVIEW,
	PAGE_IRC_CHANNEL
} page_type_t;

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
	uint16_t scroll_x;
};

typedef struct page page_t;

struct message_hdr
{
	uint8_t page_id;
	uint8_t type;
	uint8_t channel;
} __attribute__((packed));
typedef struct message_hdr message_hdr_t;

void ui_render_logs(page_t *page);
void ui_render_statistics(page_t *page);
void ui_render_irc(page_t *page);
const void *ui_objectlog_get_message(objectlog_t *log, int object_idx, unsigned offset, objectlog_iterator_t *riter, message_hdr_t *hdr, uint8_t *len);

extern page_t *current_page;
extern const page_t pages[4];