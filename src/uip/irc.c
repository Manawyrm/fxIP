#include "irc.h"

#include "psock.h"
#include "uip.h"
#include "ui.h"

#include <string.h>

#define _GNU_SOURCE
#include "fxlibc/string.h"

static char *localhostname;

static const char irc_command_privmsg[] = "PRIVMSG";
static const char irc_command_error[] = "ERROR";

static int irc_parse_command(irc_message message)
{
	char *whitespace;

	if (!strlen(message.command))
	{
		ui_write_log(PAGE_IRC_OVERVIEW, 0, 0, "empty cmd", 11);
		return; 
	}

	if (strncasecmp(message.command, irc_command_privmsg, strlen(message.command)) == 0)
	{
		//ui_write_log(PAGE_IRC_CHANNEL, 0, 0, "privmsg", 7);

		whitespace = memchr(message.command_arguments, ' ', strlen(message.command_arguments));
		if (whitespace == NULL)
			return;

		ui_write_log(PAGE_IRC_CHANNEL, 0, 0, &whitespace[2], strlen(&whitespace[2]) - 1);

		return;
	}
	else
	{
		ui_write_log(PAGE_IRC_OVERVIEW, 0, 0, message.command_arguments, strlen(message.command_arguments));
	}
	
}

static int irc_parse_line(char *data, uint16_t length)
{
	char *whitespace;

	data[length - 1] = 0x00;

	irc_message message;
	message.message = data;
	message.length = length;
	message.prefix = NULL;
	message.message_without_prefix = NULL;
	message.command = NULL;
	message.command_arguments = NULL;

	// Prefix present?
	if (data[0] == ':')
	{
		whitespace = memchr(data, ' ', length);
		if (whitespace == NULL)
		{
			ui_write_log(PAGE_IRC_OVERVIEW, 0, 0, "no ws found", 11);
			return;
		}

		message.prefix = &data[1];
		whitespace[0] = 0x00;

		message.message_without_prefix = &whitespace[1];
	}
	else
	{
		message.message_without_prefix = data;
	}

	// split message into command and command args
	whitespace = memchr(message.message_without_prefix, ' ', strlen(message.message_without_prefix));
	if (whitespace == NULL)
	{
		ui_write_log(PAGE_IRC_OVERVIEW, 0, 0, "no ws found", 11);
		return;
	}
	message.command = &message.message_without_prefix[0];
	whitespace[0] = 0x00;

	message.command_arguments = &whitespace[1];

	irc_parse_command(message);
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