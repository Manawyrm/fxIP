#include <stdlib.h>
#include <gint/rtc.h>

#include "network.h"
#include "uip/uip.h"
#include "uip/timer.h"
#include "clock-arch.h"

#include "log.h"
#include "slipdev.h"
#include "ui.h"

uip_ipaddr_t ipaddr;
struct timer periodic_timer, arp_timer;

void network_init()
{
	slipdev_init();
	timer_set(&periodic_timer, CLOCK_SECOND / 2);

	srand(rtc_ticks());

	uip_init();
	fxip_log("uip_init() done");

	uip_ipaddr(ipaddr, 10,10,10,2);
	uip_sethostaddr(ipaddr);
	uip_ipaddr(ipaddr, 10,10,10,1);
	uip_setdraddr(ipaddr);
	uip_ipaddr(ipaddr, 255,255,255,0);
	uip_setnetmask(ipaddr);

	//httpd_init();
	irc_init();

	uip_ipaddr(ipaddr, 163,123,192,192);
	//uip_ipaddr(ipaddr, 10,10,10,1);
	irc_connect(ipaddr);

	fxip_log("app_init() done");

}

void network_poll()
{
	uip_len = slipdev_poll();
	if(uip_len > 0)
	{
		uip_input();
		/* If the above function invocation resulted in data that
			should be sent out on the network, the global variable
			uip_len is set to a value > 0. */
		if(uip_len > 0)
		{
			slipdev_send();
		}

	}
	else if(timer_expired(&periodic_timer))
	{
		timer_reset(&periodic_timer);

		for(int i = 0; i < UIP_CONNS; i++)
		{
			uip_periodic(i);
			/* If the above function invocation resulted in data that
				should be sent out on the network, the global variable
				uip_len is set to a value > 0. */
			if(uip_len > 0)
			{
				slipdev_send();
			}
		}
	}

}