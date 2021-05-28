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
  IRC_CONNECTION_NICK,
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


#ifndef UIP_APPCALL
#define UIP_APPCALL irc_appcall
#endif


  /*PSOCK_READTO(&s.psock, ISO_nl);
   
  if(strncmp(s.inputbuffer, irc_220, 3) != 0) {
    PSOCK_CLOSE(&s.psock);
    irc_done(2);
    PSOCK_EXIT(&s.psock);
  }
  
  PSOCK_SEND_STR(&s.psock, (char *)irc_helo);
  PSOCK_SEND_STR(&s.psock, localhostname);
  PSOCK_SEND_STR(&s.psock, (char *)irc_crnl);

  PSOCK_READTO(&s.psock, ISO_nl);
  
  if(s.inputbuffer[0] != ISO_2) {
    PSOCK_CLOSE(&s.psock);
    irc_done(3);
    PSOCK_EXIT(&s.psock);
  }

  PSOCK_SEND_STR(&s.psock, (char *)irc_mail_from);
  PSOCK_SEND_STR(&s.psock, s.from);
  PSOCK_SEND_STR(&s.psock, (char *)irc_crnl);

  PSOCK_READTO(&s.psock, ISO_nl);
  
  if(s.inputbuffer[0] != ISO_2) {
    PSOCK_CLOSE(&s.psock);
    irc_done(4);
    PSOCK_EXIT(&s.psock);
  }

  PSOCK_SEND_STR(&s.psock, (char *)irc_rcpt_to);
  PSOCK_SEND_STR(&s.psock, s.to);
  PSOCK_SEND_STR(&s.psock, (char *)irc_crnl);

  PSOCK_READTO(&s.psock, ISO_nl);
  
  if(s.inputbuffer[0] != ISO_2) {
    PSOCK_CLOSE(&s.psock);
    irc_done(5);
    PSOCK_EXIT(&s.psock);
  }
  
  if(s.cc != 0) {
    PSOCK_SEND_STR(&s.psock, (char *)irc_rcpt_to);
    PSOCK_SEND_STR(&s.psock, s.cc);
    PSOCK_SEND_STR(&s.psock, (char *)irc_crnl);

    PSOCK_READTO(&s.psock, ISO_nl);
  
    if(s.inputbuffer[0] != ISO_2) {
      PSOCK_CLOSE(&s.psock);
      irc_done(6);
      PSOCK_EXIT(&s.psock);
    }
  }
  
  PSOCK_SEND_STR(&s.psock, (char *)irc_data);
  
  PSOCK_READTO(&s.psock, ISO_nl);
  
  if(s.inputbuffer[0] != ISO_3) {
    PSOCK_CLOSE(&s.psock);
    irc_done(7);
    PSOCK_EXIT(&s.psock);
  }

  PSOCK_SEND_STR(&s.psock, (char *)irc_to);
  PSOCK_SEND_STR(&s.psock, s.to);
  PSOCK_SEND_STR(&s.psock, (char *)irc_crnl);
  
  if(s.cc != 0) {
    PSOCK_SEND_STR(&s.psock, (char *)irc_cc);
    PSOCK_SEND_STR(&s.psock, s.cc);
    PSOCK_SEND_STR(&s.psock, (char *)irc_crnl);
  }
  
  PSOCK_SEND_STR(&s.psock, (char *)irc_from);
  PSOCK_SEND_STR(&s.psock, s.from);
  PSOCK_SEND_STR(&s.psock, (char *)irc_crnl);
  
  PSOCK_SEND_STR(&s.psock, (char *)irc_subject);
  PSOCK_SEND_STR(&s.psock, s.subject);
  PSOCK_SEND_STR(&s.psock, (char *)irc_crnl);

  PSOCK_SEND(&s.psock, s.msg, s.msglen);
  
  PSOCK_SEND_STR(&s.psock, (char *)irc_crnlperiodcrnl);

  PSOCK_READTO(&s.psock, ISO_nl);
  if(s.inputbuffer[0] != ISO_2) {
    PSOCK_CLOSE(&s.psock);
    irc_done(8);
    PSOCK_EXIT(&s.psock);
  }

  PSOCK_SEND_STR(&s.psock, (char *)irc_quit);
  irc_done(irc_ERR_OK);
  */
