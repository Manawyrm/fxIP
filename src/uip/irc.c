#include "irc.h"

#include "psock.h"
#include "uip.h"
#include "ui.h"

#include <string.h>

static char *localhostname;

static int irc_parse_line(char *data, uint16_t length)
{
	ui_write_log(PAGE_IRC, 0, 0, data, length );
}

/*---------------------------------------------------------------------------*/
static int irc_thread(struct irc_state *s)
{
	PSOCK_BEGIN(&s->psock);

	PSOCK_SEND_STR(&s->psock, "nick CasioFXIRC\r\n");
	PSOCK_SEND_STR(&s->psock, "user CasioFXIRC 0 0 CasioFX\r\n");
	PSOCK_READTO(&s->psock, '\n');
	PSOCK_SEND_STR(&s->psock, "JOIN ##manawyrmtest\r\n");
	PSOCK_READTO(&s->psock, '\n');


	while (1)
	{
		PSOCK_READTO(&s->psock, '\n');
		irc_parse_line(s->psock.bufptr, PSOCK_DATALEN(&s->psock));
	}

	PSOCK_END(&s->psock);
}

int irc_send_quit_thread(struct irc_state *s)
{
	PSOCK_BEGIN(&s->psock);

	PSOCK_SEND_STR(&s->psock, "QUIT meow!\r\n");

	PSOCK_END(&s->psock);
}

/*---------------------------------------------------------------------------*/

void irc_exit(void)
{
	struct irc_state *s = &(uip_conn->appstate);

	if (&s->psock == NULL)
	{
		return;
	}
	irc_send_quit_thread(s);
}

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

	irc_thread(&(uip_conn->appstate));
}

unsigned char irc_connect(u16_t *server)
{
	struct uip_conn *conn;

	conn = uip_connect(server, HTONS(6667));
	if(conn == NULL) {
		return 0;
	}

	struct irc_state *s = &conn->appstate;
	s->connected = 1;
	s->connection_state = IRC_CONNECTION_NEW;

	PSOCK_INIT(&s->psock, s->inputbuffer, 1024);

	return 1;
}
/*---------------------------------------------------------------------------*/
void
irc_init(void)
{
}
/*---------------------------------------------------------------------------*/
/** @} */
/** @} */