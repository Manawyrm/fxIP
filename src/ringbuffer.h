#pragma once
#include <stdint.h>
#include <string.h>

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

static inline void ringbuffer_write(ringbuffer_t *ring, const void *data, uint16_t len)
{
	if (ring->size - ring->write_ptr < len)
	{
		const uint8_t *data8 = data;
		if (len > ring->size)
		{
			data8 += len - ring->size;
			len = ring->size;
		}

		memcpy(&ring->buffer[ring->write_ptr], data8, ring->size - ring->write_ptr);
		data8 += ring->size - ring->write_ptr;
		len -= ring->size - ring->write_ptr;
		ring->write_ptr = 0;

		memcpy(&ring->buffer[ring->write_ptr], data8, len);
		ring->write_ptr += len;
	}
	else
	{
		memcpy(&ring->buffer[ring->write_ptr], data, len);
		ring->write_ptr += len;
	}

	ring->write_ptr %= ring->size;
}

static inline void ringbuffer_advance_read(ringbuffer_t *ring, uint16_t len)
{
	if (len > ring->size)
	{
		len -= ring->size;
	}

	ring->read_ptr += len;
	ring->read_ptr %= ring->size;
}
