#include "ui-keyboard.h"
#include "ui.h"

#include <gint/keyboard.h>

#include "util.h"

uint8_t keyboard_input_buffer[128];
uint8_t keyboard_input_bufpos = 0;

uint8_t shift = 0; 
uint8_t alpha = 0;

/* keycode_character(): Identify keys A .. Z, space and doublequotes */
int ui_keycode_character(int keycode)
{
	// Non-Alpha Mode (0-9, etc.)
	if (alpha)
	{
		if (keycode >= KEY_7 && keycode <= KEY_9)
			return '7' + (keycode - KEY_7);

		if (keycode >= KEY_4 && keycode <= KEY_6)
			return '4' + (keycode - KEY_4);

		if (keycode >= KEY_1 && keycode <= KEY_3)
			return '1' + (keycode - KEY_1);

		if (keycode == KEY_0)	return '0';
		if (keycode == KEY_DOT)	return '.';
		if (keycode == KEY_ADD)	return '+';
		if (keycode == KEY_SUB)	return '-';
		if (keycode == KEY_MUL)	return '*';
		if (keycode == KEY_DIV)	return '/';
		if (keycode == KEY_LEFTP)	return '(';
		if (keycode == KEY_RIGHTP)	return ')';
		if (keycode == KEY_COMMA)	return ',';

		return -1;
	}

	uint8_t character = 0;

	if (keycode >= KEY_XOT && keycode <= KEY_TAN)
		character = 'A' + (keycode - KEY_XOT);

	if (keycode >= KEY_FRAC && keycode <= KEY_ARROW)
		character = 'G' + (keycode - KEY_FRAC);

	if (keycode >= KEY_7 && keycode <= KEY_9)
		character = 'M' + (keycode - KEY_7);

	if (keycode >= KEY_4 && keycode <= KEY_DIV)
		character = 'P' + (keycode - KEY_4);

	if (keycode >= KEY_1 && keycode <= KEY_SUB)
		character = 'U' + (keycode - KEY_1);

	if (keycode == KEY_0)	character = 'Z';
	if (keycode == KEY_DOT)	character = ' ';	
	if (keycode == KEY_EXP)	character = '"';	

	if (!shift)
	{
		if (character >= 'A' && character <= 'Z')
		{
			return character + ('a' - 'A');
		}
	}

	if (character)
	{
		return character;
	}

	return -1;
}

int ui_handle_keyboard()
{
	// keyboard input
	int character = -1;
	key_event_t event = pollevent();
	if (event.type == KEYEV_DOWN)
	{
		// Modifiers
		if (event.key == KEY_SHIFT)
		{
			shift = 1;
		}
		if (event.key == KEY_ALPHA)
		{
			alpha = 1;
		}
	}
	if (event.type == KEYEV_UP)
	{
		// Modifiers
		if (event.key == KEY_SHIFT)
		{
			shift = 0;
		}
		if (event.key == KEY_ALPHA)
		{
			alpha = 0;
		}

		// Page selection
		for (unsigned int pageid = 0; pageid < ARRAY_SIZE(pages); pageid++)
		{
			if (pages[pageid].key == event.key)
			{
				current_page = pages[pageid];
				ui_update();
			}
		}

		if (event.key == KEY_EXIT)
		{
			return -1;
		}

		// Text input
		if ((character = ui_keycode_character(event.key)) >= 0)
		{
			// input buffer full
			if (keyboard_input_bufpos == sizeof(keyboard_input_buffer) - 1)
				return 0;

			keyboard_input_buffer[keyboard_input_bufpos] = character;
			keyboard_input_bufpos++;
		}
		if (event.key == KEY_DEL)
		{
			keyboard_input_buffer[keyboard_input_bufpos - 1] = 0x00;
			keyboard_input_bufpos--;
		}
		if (event.key == KEY_EXE)
		{
			keyboard_input_buffer[keyboard_input_bufpos] = 0x00;
			fxip_printf("%s", keyboard_input_buffer);
			keyboard_input_bufpos = 0;
		}

	}
	return 0;
}
