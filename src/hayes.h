#pragma once
#include <stdint.h>

int hayes_send_and_read(char *command, char* readbuf, char* readbuflength);