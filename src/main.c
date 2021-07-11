#include <gint/gint.h>
#include <gint/std/stdio.h>
#include <gint/std/string.h>
#include <gint/std/stdlib.h>

#include "util.h"
#include "scif.h"
#include "ui.h"
#include "log.h"
#include "network.h"
#include "clock-arch.h"
#include "hayes.h"
#include "uip/irc.h"

int __Serial_Open                    (unsigned char *mode);
int __Serial_Close                   (int mode);

static int casioos_Serial_Init()
{
	 unsigned char settings[5]={0,9,0,0,0};
	 __Serial_Open(settings);
	 return 0;
}
static int casioos_Serial_Close()
{
	__Serial_Close(1);
	 return 0;
}

//uint8_t atdt_call_buffer[128];

int main(void)
{
	gint_world_switch(GINT_CALL(casioos_Serial_Init));
	scif_init();

	ui_init();
	fxip_log("");
	fxip_log("fxIP, build date:");
	fxip_log(&__TIMESTAMP__[4]);
	fxip_log("manawyrm & TobleMiner");

	clock_setup();

	/*while (hayes_send_and_read("ATI3", atdt_call_buffer, sizeof(atdt_call_buffer)) == 0)
	{
		uint32_t start = clock_time();
		while (start + 1 > clock_time());
	}
	fxip_log("read 1:");
	fxip_log(atdt_call_buffer);

	fxip_log("Calling **2");

	scif_read_flush();
	hayes_send("ATDT **2");
	
	while (true)
	{
		hayes_read(atdt_call_buffer, sizeof(atdt_call_buffer), 0, 1);
		fxip_log("read 2:");
		fxip_log(atdt_call_buffer);
	}*/

	network_init();

	uint32_t last_clock_time = 0;
	while (true)
	{
		network_poll();

		if (ui_handle_keyboard())
		{
			// Exit
			if (!messagelength)
			{
				memcpy(messagebuffer, "QUIT", 4);
				messagelength = 4;

				uint32_t now = clock_time();
				while ((clock_time() - now < 50) && messagelength)
				{
					network_poll();
				}
			}
		
			gint_world_switch(GINT_CALL(casioos_Serial_Close));
			return 1;
		}

		uint32_t now = clock_time();
		if (last_clock_time != now && now % 5 == 0)
		{
			ui_cursor_blink();
			ui_update();
			last_clock_time = now;
		}
	}

	return 1;
}