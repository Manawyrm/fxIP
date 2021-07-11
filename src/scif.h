#pragma once 
#include "stdint.h"

void scif_debug();
void scif_write(const void *data, uint16_t len);
int scif_read();
void scif_init();
int scif_read_flush();