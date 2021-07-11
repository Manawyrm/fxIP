#include <stdbool.h>
#include <string.h>

#include "objectlog.h"

#define MAX_FRAGMENT_LEN 0x7f
#define FRAGMENT_FINAL 0x80
#define FRAGMENT_LEN(x) ((x)&MAX_FRAGMENT_LEN)

#define DIV_ROUND_UP(x, y) (((x) + ((y)-1)) / (y))

static uint16_t get_next_entry(objectlog_t *log, uint16_t offset)
{
	uint8_t fragment_hdr;

	log->ring.read_ptr = offset;
	do
	{
		uint16_t hdr_pos = log->ring.read_ptr;

		fragment_hdr = ringbuffer_read_one(&log->ring);
		ringbuffer_advance_read(&log->ring, FRAGMENT_LEN(fragment_hdr));
		/*
		 * There might be no terminating entry in the list. Detect
		 * whether we have wrapped across @offset and terminate if
		 * we did
		 */
		if ((hdr_pos < offset && log->ring.read_ptr >= offset) ||
			(hdr_pos > offset && log->ring.read_ptr >= offset &&
			 log->ring.read_ptr < hdr_pos))
		{
			return log->ptr_first;
		}
	} while (!(fragment_hdr & FRAGMENT_FINAL));

	return log->ring.read_ptr;
}

static uint16_t objectlog_space_between(objectlog_t *log, uint16_t first, uint16_t second)
{
	if (second >= first)
	{
		return second - first;
	}
	else
	{
		return log->ring.size - first + second;
	}
}

static uint16_t drop_first_entry(objectlog_t *log)
{
	log->ptr_first = get_next_entry(log, log->ptr_first);
	log->num_entries--;
	return log->ptr_first;
}

static uint16_t objectlog_free_space(objectlog_t *log, uint16_t from)
{
	return objectlog_space_between(log, from, log->ptr_first);
}

static void objectlog_write_fragment_hdr(objectlog_t *log, uint16_t len, bool final)
{
	uint8_t hdr = FRAGMENT_LEN(len);

	if (final)
	{
		hdr |= FRAGMENT_FINAL;
	}
	ringbuffer_write_one(&log->ring, hdr);
}

static void objectlog_write_fragment_data(objectlog_t *log, const void *data, uint16_t len)
{
	ringbuffer_write(&log->ring, data, len);
}

void objectlog_init(objectlog_t *log, void *storage, uint16_t size)
{
	ringbuffer_init(&log->ring, storage, size);
	log->ptr_first = 0;
	log->ptr_last = 0;
	log->num_entries = 0;
}

/**
 * Write object from non-contuguous memory area to object log
 * Oftentimes data that needs to be stored is not available from a contiguous
 * memory region. This method accepts a list of (pointer, length) pairs and
 * constructs the object to be stored by iterating over it. In each iteration
 * @length bytes read from @pointer are appended to the object log.
 *
 * @returns: 0 on success, number of bytes missing for storage on failure
 */
uint16_t objectlog_write_scattered_object(objectlog_t *log, const scatter_object_t *scatter_list)
{
	const scatter_object_t *sc_list = scatter_list;
	const uint8_t *data8;
	uint16_t num_fragments;
	uint16_t data_len = 0;
	uint16_t total_len;
	uint16_t new_last;
	uint16_t free_space;
	uint16_t log_end = get_next_entry(log, log->ptr_last);
	uint16_t scatter_entry_offset = 0;
	uint16_t fragment_offset = 0;
	uint16_t fragment_len;

	/* Calculate total length of all data in @scatter_list */
	while (sc_list->len)
	{
		data_len += sc_list++->len;
	}

	/* Calculate number of fragments required to store data */
	num_fragments = DIV_ROUND_UP(data_len, MAX_FRAGMENT_LEN);
	total_len = data_len + num_fragments;
	/* Special case: If we wrap we need an extra header! */
	if (log_end + total_len > log->ring.size)
	{
		total_len++;
	}
	/* We can not store any messages exceeding size of this buffer */
	if (total_len > log->ring.size)
	{
		return log->ring.size - total_len;
	}

	/* Get number of bytes not in use at the moment */
	free_space = objectlog_free_space(log, log_end);
	/* Delete entries from start of list until object fits */
	while (free_space < total_len && log->ptr_first != log->ptr_last)
	{
		drop_first_entry(log);
		free_space = objectlog_free_space(log, log_end);
	}

	/* Store start of object header */
	new_last = log->ring.write_ptr;

	/* Write object as @num_fragments fragments */
	sc_list = scatter_list;
	data8 = sc_list->ptr;
	while (sc_list->len)
	{
		uint16_t write_len = data_len;

		/* Calculate maximum permissible size for this fragment */
		if (!fragment_offset)
		{
			fragment_len = MAX_FRAGMENT_LEN;
			/* Ensure fragment does not wrap in ring buffer */
			if (fragment_len > log->ring.size - log->ring.write_ptr - 1)
			{
				fragment_len = log->ring.size - log->ring.write_ptr - 1;
			}
		}

		/* Limit write size to fragment length */
		if (write_len + fragment_offset > fragment_len)
		{
			write_len = fragment_len - fragment_offset;
		}

		/* Write fragment header whenever we start a new fragment */
		if (!fragment_offset)
		{
			objectlog_write_fragment_hdr(log, write_len, data_len == write_len);
		}

		/* Limit length of this write to scatter entry length */
		if (write_len > sc_list->len - scatter_entry_offset)
		{
			write_len = sc_list->len - scatter_entry_offset;
		}

		objectlog_write_fragment_data(log, data8, write_len);
		data8 += write_len;
		data_len -= write_len;
		scatter_entry_offset += write_len;
		fragment_offset += write_len;

		/* Switch to next scatter entry if there is no more data in current one */
		if (scatter_entry_offset >= sc_list->len)
		{
			sc_list++;
			data8 = sc_list->ptr;
			scatter_entry_offset = 0;
		}
		/* Start new fragment if there is no more space in current one */
		if (fragment_offset >= fragment_len)
		{
			fragment_offset = 0;
		}
	}
	log->ptr_last = new_last;
	log->num_entries++;

	return 0;
}

uint16_t objectlog_write_object(objectlog_t *log, const void *data, uint16_t len)
{
	scatter_object_t scatter_list[] = {
		{.ptr = data, .len = len},
		{.len = 0}};

	return objectlog_write_scattered_object(log, scatter_list);
}

uint16_t objectlog_write_string(objectlog_t *log, const char *str)
{
	return objectlog_write_object(log, str, strlen(str));
}

/**
 * Obtain interator for object at index @object_idx
 * Negative indices count from last to first string.
 *
 * @returns: non-negative iterator value on success, -1 on failure
 */
objectlog_iterator_t objectlog_iterator(objectlog_t *log, int object_idx)
{
	uint16_t string_ptr = log->ptr_first;

	if (object_idx < 0)
	{
		object_idx = -object_idx;
		if (object_idx >= log->num_entries)
		{
			return -1;
		}
		object_idx = log->num_entries - object_idx;
	}
	else
	{
		if (object_idx >= log->num_entries)
		{
			return -1;
		}
	}

	while (object_idx--)
	{
		string_ptr = get_next_entry(log, string_ptr);
	}

	return string_ptr;
}

/**
 * Get current iteration fragment
 *
 * @returns: non-NULL pointer to data on success, NULL on failure
 */
const void *objectlog_get_fragment(objectlog_t *log, objectlog_iterator_t iterator, uint8_t *len)
{
	if (iterator < 0)
	{
		return NULL;
	}

	if (iterator > log->ring.size)
	{
		return NULL;
	}

	*len = FRAGMENT_LEN(log->ring.buffer[iterator]);
	return &log->ring.buffer[iterator + 1];
}

/**
 * Advance iterator to next fragment
 *
 * @returns: -2 if there are no more fragments or
 *	     non-negative iterator value of next fragment or
 *	     -1 on failure
 */
objectlog_iterator_t objectlog_next(objectlog_t *log, objectlog_iterator_t iterator)
{
	uint16_t len;

	if (iterator < 0)
	{
		return -1;
	}

	if (iterator > log->ring.size)
	{
		return -1;
	}

	len = FRAGMENT_LEN(log->ring.buffer[iterator]);
	if (log->ring.buffer[iterator] & FRAGMENT_FINAL)
	{
		return -2;
	}

	iterator++;
	iterator += len;
	iterator %= log->ring.size;
	return iterator;
}

/**
 * Get size of object at index @object_idx
 *
 * @returns: -1 on failure, else
 *	     non-negative length of object
 */
long objectlog_get_object_size(objectlog_t *log, int object_idx)
{
	objectlog_iterator_t iter;
	uint16_t len = 0;

	iter = objectlog_iterator(log, object_idx);
	if (iter < 0)
	{
		return -1;
	}
	while (iter >= 0)
	{
		uint8_t fragment_size;

		if (!objectlog_get_fragment(log, iter, &fragment_size))
		{
			return -1;
		}
		len += fragment_size;
		iter = objectlog_next(log, iter);
	}

	return len;
}