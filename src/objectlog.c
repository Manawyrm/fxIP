#include <stdbool.h>
#include <string.h>

#include "objectlog.h"

#define MAX_FRAGMENT_LEN 0x7f
#define FRAGMENT_FINAL 0x80
#define FRAGMENT_LEN(x) ((x) & MAX_FRAGMENT_LEN)

#define DIV_ROUND_UP(x, y) (((x) + ((y) - 1)) / (y))

static void write_fragment(objectlog_t *log, uint8_t hdr, const void *data, uint16_t len) {
	ringbuffer_write_one(&log->ring, hdr);
	ringbuffer_write(&log->ring, data, len);
}

static uint16_t get_next_entry(objectlog_t *log, uint16_t offset) {
	uint8_t fragment_hdr;

	log->ring.read_ptr = offset;
	do {
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
		     log->ring.read_ptr < hdr_pos)) {
			return log->ptr_first;
		}
	} while (!(fragment_hdr & FRAGMENT_FINAL));

	return log->ring.read_ptr;
}

static uint16_t objectlog_space_between(objectlog_t *log, uint16_t first, uint16_t second) {
	if (second >= first) {
		return second - first;
	} else {
		return log->ring.size - first + second;
	}
}

static uint16_t drop_first_entry(objectlog_t *log) {
	log->ptr_first = get_next_entry(log, log->ptr_first);
	log->num_entries--;
	return log->ptr_first;
}

static uint16_t objectlog_free_space(objectlog_t *log, uint16_t from) {
	return objectlog_space_between(log, from, log->ptr_first);
}

static void objectlog_add_fragment(objectlog_t *log, const void *data, uint16_t len, bool final) {
	uint8_t hdr = FRAGMENT_LEN(len);

	if (final) {
		hdr |= FRAGMENT_FINAL;
	}
	write_fragment(log, hdr, data, len);
}

void objectlog_init(objectlog_t *log, void *storage, uint16_t size) {
	ringbuffer_init(&log->ring, storage, size);
	log->ptr_first = 0;
	log->ptr_last = 0;
	log->num_entries = 0;
}

void objectlog_write_object(objectlog_t *log, const void *data, uint16_t len) {
	const uint8_t *data8 = data;
	uint16_t num_fragments;
	uint16_t total_len;
	uint16_t new_last;
	uint16_t free_space;
	uint16_t log_end = get_next_entry(log, log->ptr_last);

	/* Calculate number of fragments required to store data */
	num_fragments = DIV_ROUND_UP(len, MAX_FRAGMENT_LEN);
	total_len = len + num_fragments;
	/* Special case: If we wrap we need an extra header! */
	if (log_end + total_len > log->ring.size) {
		total_len++;
	}
	/* We can not store any messages exceeding size of this buffer */
	if (total_len > log->ring.size) {
		return;
	}

	/* Get number of bytes not in use at the moment */
	free_space = objectlog_free_space(log, log_end);
	/* Delete entries from start of list until object fits */
	while (free_space < total_len && log->ptr_first != log->ptr_last) {
		drop_first_entry(log);
		free_space = objectlog_free_space(log, log_end);
	}

	/* Store start of object header */
	new_last = log->ring.write_ptr;
	/* Write object as @num_fragments fragments */
	while(len) {
		uint16_t fragment_len = len;
		/* Limit fragment size to value representable in 7 bits */
		if (fragment_len > MAX_FRAGMENT_LEN) {
			fragment_len = MAX_FRAGMENT_LEN;
		}
		/* Don't create fragments wrapping around the end of ring buffer */
		if (fragment_len + 1 > log->ring.size - log->ring.write_ptr) {
			fragment_len = log->ring.size - log->ring.write_ptr - 1;
		}
		objectlog_add_fragment(log, data8, fragment_len, fragment_len == len);
		len -= fragment_len;
		data8 += fragment_len;
	}
	log->ptr_last = new_last;
	log->num_entries++;
}

void objectlog_write_string(objectlog_t *log, const char *str) {
	objectlog_write_object(log, str, strlen(str));
}

/**
 * Obtain interator for object at index @object_idx
 * Negative indices count from last to first string.
 *
 * @returns: non-negative iterator value on success, -1 on failure
 */
objectlog_iterator_t objectlog_iterator(objectlog_t *log, int object_idx) {
	uint16_t string_ptr = log->ptr_first;

	if (object_idx < 0) {
		object_idx = -object_idx;
		if (object_idx >= log->num_entries) {
			return -1;
		}
		object_idx = log->num_entries - object_idx;
	} else {
		if (object_idx >= log->num_entries) {
			return -1;
		}

	}

	while (object_idx--) {
		string_ptr = get_next_entry(log, string_ptr);
	}

	return string_ptr;
}

/**
 * Get current iteration fragment
 *
 * @returns: non-NULL pointer to data on success, NULL on failure
 */
const void *objectlog_get_fragment(objectlog_t *log, objectlog_iterator_t iterator, uint8_t *len) {
	if (iterator < 0) {
		return NULL;
	}

	if (iterator > log->ring.size) {
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
objectlog_iterator_t objectlog_next(objectlog_t *log, objectlog_iterator_t iterator) {
	uint16_t len;

	if (iterator < 0) {
		return -1;
	}

	if (iterator > log->ring.size) {
		return -1;
	}

	len = FRAGMENT_LEN(log->ring.buffer[iterator]);
	if (log->ring.buffer[iterator] & FRAGMENT_FINAL) {
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
long objectlog_get_object_size(objectlog_t *log, int object_idx) {
	objectlog_iterator_t iter;
	uint16_t len = 0;

	iter = objectlog_iterator(log, object_idx);
	if (iter < 0) {
		return -1;
	}
	while (iter >= 0) {
		uint8_t fragment_size;

		if (!objectlog_get_fragment(log, iter, &fragment_size)) {
			return -1;
		}
		len += fragment_size;
		iter = objectlog_next(log, iter);
	}

	return len;
}
