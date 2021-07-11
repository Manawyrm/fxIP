#pragma once
#include <stdint.h>

int hayes_send_and_read(char *command, char *readbuf, uint16_t readbuflength);
int hayes_read(char *readbuf, uint16_t readbuflength, int timeout, int ignore_echo);
int hayes_send(char *command);