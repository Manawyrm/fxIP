#pragma once
#include "uipopt.h"
#include "psock.h"

void irc_init(void);

/* Functions. */
void irc_configure(u16_t *server);

void irc_appcall(void);
void irc_done(unsigned char error);
unsigned char irc_connect(u16_t *server);

typedef enum { 
  IRC_CONNECTION_NEW,
  IRC_CONNECTION_USER,
  IRC_CONNECTION_JOIN,
  IRC_CONNECTION_ESTABLISHED
} irc_connection_state_t; 


typedef struct irc_state
{
  u8_t state;
  irc_connection_state_t connection_state;

  uint8_t connected;
  struct psock psock;
  uint8_t inputbuffer[128];
} uip_tcp_appstate_t;

typedef struct {
  char *message;  // :tungsten.libera.chat 250 asdfCasio :Highest connection count: 6972 (6971 clients) (71075 connections received)
  uint16_t length;

  char *prefix;   // tungsten.libera.chat
  char *message_without_prefix; // 250 asdfCasio :Highest connection count: 6972 (6971 clients) (71075 connections received)
  char *command; // 250
  char *command_arguments;
} irc_message;

#ifndef UIP_APPCALL
#define UIP_APPCALL irc_appcall
#endif