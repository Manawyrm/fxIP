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
	fxip_log("fxIP, build date:");
	fxip_log(&__TIMESTAMP__[4]);
	fxip_log("manawyrm & TobleMiner");

	gint_world_switch(GINT_CALL(casioos_Serial_Init));
	scif_init();

	network_init();

	while (true)
	{
		network_poll();

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
	}

	return 1;
}