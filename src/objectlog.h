#pragma once

#include <stddef.h>

#include "ringbuffer.h"

typedef struct {
	ringbuffer_t ring;
	uint16_t ptr_first;
	uint16_t ptr_last;
	unsigned int num_entries;
} objectlog_t;

typedef long objectlog_iterator_t;

void objectlog_init(objectlog_t *log, void *storage, uint16_t size);
void objectlog_write_object(objectlog_t *log, const void *data, uint16_t len);
void objectlog_write_string(objectlog_t *log, const char *str);
objectlog_iterator_t objectlog_iterator(objectlog_t *log, int object_idx);
const void *objectlog_get_fragment(objectlog_t *log, objectlog_iterator_t iterator, uint8_t *len);
objectlog_iterator_t objectlog_next(objectlog_t *log, objectlog_iterator_t iterator);
long objectlog_get_object_size(objectlog_t *log, int object_idx);
