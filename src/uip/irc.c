#include "irc.h"

#include "psock.h"
#include "uip.h"

#include <string.h>

static char *localhostname;

#define ISO_nl 0x0a
#define ISO_cr 0x0d

#define ISO_period 0x2e

#define ISO_2  0x32
#define ISO_3  0x33
#define ISO_4  0x34
#define ISO_5  0x35


/*---------------------------------------------------------------------------*/
static int irc_thread(struct irc_state *s)
{
	PSOCK_BEGIN(&s->psock);
	PSOCK_SEND_STR(&s->psock, "nick CasioFXIRC\r\n");
	PSOCK_SEND_STR(&s->psock, "user CasioFXIRC 0 0 CasioFX\r\n");
	PSOCK_READTO(&s->psock, '\n');

	PSOCK_SEND_STR(&s->psock, "JOIN ##manawyrmtest");

	while (1)
	{
		PSOCK_READTO(&s->psock, '\n');
		//fxip_printf("finixed readto", s->connection_state);
	}

	PSOCK_END(&s->psock);
}
/*---------------------------------------------------------------------------*/
uint32_t counter = 0;
void irc_appcall(void)
{
	struct irc_state *s = &(uip_conn->appstate);

	if (&s->psock == NULL)
	{
		return;
	}
	if(uip_closed())
	{
		s->connected = 0;
		return;
	}
	if(uip_aborted() || uip_timedout())
	{
		s->connected = 0;
		return;
	}

	//fxip_printf("pt lc: %u", s->psock.pt.lc);
	//fxip_printf("psock lc: %u", s->psock.psockpt.lc);

	irc_thread(&(uip_conn->appstate));
}

unsigned char irc_connect(u16_t *server)
{
	struct uip_conn *conn;
	struct irc_state *s = &(uip_conn->appstate);

	conn = uip_connect(server, HTONS(6667));
	if(conn == NULL) {
		return 0;
	}
	s->connected = 1;

	PSOCK_INIT(&s->psock, s->inputbuffer, 1024);
	
	return 1;
}
/*---------------------------------------------------------------------------*/
void
irc_init(void)
{
	struct irc_state *s = &(uip_conn->appstate);
	s->connection_state = IRC_CONNECTION_NEW; 
	s->connected = 0;
}
/*---------------------------------------------------------------------------*/
/** @} */
/** @} */