#include "irc.h"

#include "psock.h"
#include "uip.h"
#include "ui.h"
#include "util.h"

#include <string.h>

#define _GNU_SOURCE
#include "fxlibc/string.h"

static char *localhostname;

static const char irc_command_privmsg[] = "PRIVMSG";
static const char irc_command_error[] = "ERROR";
static const char irc_command_ping[] = "PING";
static const char irc_command_join[] = "JOIN";

const char irc_username[] = "CasioFXIRC";
int irc_username_length = 10;

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
		whitespace = memchr(message.command_arguments, ' ', strlen(message.command_arguments));
		if (whitespace == NULL)
			return;

		whitespace[2 + strlen(&whitespace[2]) - 1] = 0x00;

		// find ! in prefix (which contains the username) and end the string there
		char *prefixend = memchr(message.prefix, '!', strlen(message.prefix));
		prefixend[0] = 0x00;

		printflength = snprintf(messagebuffer, sizeof(messagebuffer) - 1, "<%s> %s", message.prefix , &whitespace[2]);
		printflength = MIN(sizeof(messagebuffer), printflength);

		ui_write_log(PAGE_IRC_CHANNEL, 0, 0, messagebuffer, printflength);
		return;
	}

	if (strncasecmp(message.command, irc_command_ping, strlen(message.command)) == 0)
	{ 
		messagelength = snprintf(messagebuffer, sizeof(messagebuffer) - 1, "PONG %s", message.command_arguments);
		messagelength = MIN(sizeof(messagebuffer), messagelength);
		return;
	}

	if (strncasecmp(message.command, irc_command_join, strlen(message.command)) == 0)
	{
		ui_write_log(PAGE_IRC_OVERVIEW, 0, 0, "JOIN successful", 15);
		return;
	}

	whitespace = memchr(message.command_arguments, ' ', strlen(message.command_arguments));
	if (whitespace == NULL)
		return;

	whitespace[2 + strlen(&whitespace[2]) - 1] = 0x00;
	ui_write_log(PAGE_IRC_OVERVIEW, 0, 0, &whitespace[2], strlen(&whitespace[2]) - 1);
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

static int irc_send_thread(struct irc_state *s)
{
	PSOCK_BEGIN(&s->psock_out);

	PSOCK_SEND(&s->psock_out, messagebuffer, messagelength);
	PSOCK_SEND_STR(&s->psock_out, "\r\n");

	messagelength = 0;

	PSOCK_END(&s->psock_out);
}

/*---------------------------------------------------------------------------*/

void irc_appcall(void)
{
	struct irc_state *s = &(uip_conn->appstate);

	if (&s->psock == NULL || &s->psock_out == NULL)
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

	if (messagelength)
	{
		irc_send_thread(&(uip_conn->appstate));
	}
}

unsigned char irc_connect(u16_t *server)
{
	struct uip_conn *conn;

	conn = uip_connect(server, HTONS(6667));
	if(conn == NULL) {
		return 0;
	}

	struct irc_state *s = &conn->appstate;
	messagelength = 0;
	s->connected = 1;
	s->connection_state = IRC_CONNECTION_NEW;

	PSOCK_INIT(&s->psock, s->inputbuffer, 1024);
	PSOCK_INIT(&s->psock_out, s->inputbuffer, 1024);

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