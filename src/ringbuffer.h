#pragma once
#include <stdint.h>

typedef struct 
{
	uint8_t *buffer;
	volatile uint16_t read_ptr;
	volatile uint16_t write_ptr;
	uint16_t size;
} ringbuffer_t;

static inline void ringbuffer_init(ringbuffer_t *ring, void *buffer, uint16_t size)
{
	ring->buffer = buffer;
	ring->read_ptr = 0;
	ring->write_ptr = 0;
	ring->size = size;
}

static inline uint16_t ringbuffer_available(ringbuffer_t *ring)
{
	if (ring->write_ptr >= ring->read_ptr)
	{
		return ring->write_ptr - ring->read_ptr;
	}
	else
	{
		return ring->size - ring->read_ptr + ring->write_ptr;
	}
}

static inline uint8_t ringbuffer_read_one(ringbuffer_t *ring)
{
	uint8_t byte = ring->buffer[ring->read_ptr++];
	ring->read_ptr %= ring->size;
	return byte;
}

static inline void ringbuffer_write_one(ringbuffer_t *ring, uint8_t byte)
{
	ring->buffer[ring->write_ptr++] = byte;
	ring->write_ptr %= ring->size;
}
