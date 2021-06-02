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


int main(void)
{
	gint_world_switch(GINT_CALL(casioos_Serial_Init));
	scif_init();

	ui_init();
	fxip_log("fxIP, build date:");
	fxip_log(&__TIMESTAMP__[4]);
	fxip_log("manawyrm & TobleMiner");

	clock_setup();

	/*uint32_t lastscifwritetime = clock_time();
	while (true)
	{
		/*if (clock_time() % 30 == 0 && lastscifwritetime != clock_time())
		{
			lastscifwritetime = clock_time();

			if (hayes_send_and_read("ATI2", datatemp, 128))
			{
				fxip_printf("recv: %s", datatemp);
			}
		}

		if (ui_handle_keyboard())
		{
			// Exit
			gint_world_switch(GINT_CALL(casioos_Serial_Close));
			return 1;
		}

		if (clock_time() % 10 == 0)
		{
			ui_update();
		}
	}*/

	network_init();

	uint32_t last_clock_time = 0;
	while (true)
	{
		network_poll();

		if (ui_handle_keyboard())
		{
			// Exit
			irc_exit();

			uint32_t now = clock_time(); 
			while (clock_time() - now < 50)
			{
				
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