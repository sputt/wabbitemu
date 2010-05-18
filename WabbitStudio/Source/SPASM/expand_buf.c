#define _GNU_SOURCE
#include "spasm.h"

#include <string.h>
#include <stdio.h>

#include <stdlib.h>
#include "expand_buf.h"
#include "utils.h"


/*
 * Initializes an expandable buffer with a
 * certain starting size
 */
//TODO: the size increase is exponential, I am not sure an init_size is necessary
expand_buf *eb_init (size_t init_size) {
	expand_buf_t *new_buf = (expand_buf_t *) malloc_chk (sizeof (expand_buf_t));
	
	if (init_size == -1) init_size = 64;
	
	new_buf->start = (unsigned char *) malloc_chk(init_size);
	new_buf->end = new_buf->start;
	new_buf->size = init_size;
	
	return new_buf;
}


int eb_append(expand_buf_t *eb, const char *text, int len) {
	return eb_insert(eb, -1, text, len);
}

/*
 * Writes text to an expandable buffer at
 * offset in buffer (-1 to append), and
 * expands if necessary, given length or
 * -1 to find length internally, returns
 * new offset
 */

int eb_insert (expand_buf *buf, int offset, const char *text, int length) {
	unsigned char *ptr;

	if (text == NULL)
		return offset;
	
	if (offset == -1)
		ptr = buf->end;
	else
		ptr = buf->start + offset;

	if (length == -1)
		length = strlen (text);

	if (length == 0)
		return 0;

	//if the string's too long for the buffer to hold, expand it
	if (buf->end + length - buf->start > buf->size) {
		unsigned char *old_start = buf->start;
		while (buf->end + length - buf->start > buf->size)
			buf->size *= 2;
		buf->start = (unsigned char *) realloc (buf->start, buf->size);
		if (buf->start == NULL) {
			puts ("Expand buf out of memory.");
			exit (1);
		}
		ptr += buf->start - old_start;
		buf->end += buf->start - old_start;
	}

	//copy the string to the buffer and move the pointer forward
	if (ptr < buf->end) {
		//if necessary, move the end of the buffer forwards first
		unsigned char *curr_ptr;
		for (curr_ptr = buf->end - 1; curr_ptr >= ptr; curr_ptr--)
			*(curr_ptr + length) = *curr_ptr;
	}
	strncpy ((char *) ptr, text, length);
	buf->end += length;

	return offset + length;
}


/*
 * Overwrites text in expandable buffer
 * starting at offset - DOES NOT
 * do length checking!
 */

void eb_overwrite (expand_buf *buf, int offset, const char *text, int length) {
	unsigned char *ptr = buf->start + offset;

	if (length == -1)
		length = strlen (text);

	strncpy ((char *) ptr, text, length);
}


/*
 * Erases text from buffer
 */

void eb_erase (expand_buf *buf, int offset, int length) {
	unsigned char *ptr, *curr_ptr;
	ptr = buf->start + offset;
	for (curr_ptr = ptr; curr_ptr < ptr + length && curr_ptr + length < buf->end; curr_ptr++)
		*curr_ptr = *(curr_ptr + length);

	buf->end -= length;
}


/*
 * Gets the char from the buffer
 * at given offset
 */

char eb_get_char (expand_buf *buf, int offset) {
	return *(buf->start + offset);
}


/*
 * Extracts the contents of the buffer as
 * a zero-terminated string
 */

char *eb_extract (expand_buf *buf) {
	return strndup ((char *) buf->start, buf->end - buf->start);
}


/*
 * Frees a buffer
 */

void eb_free (expand_buf *eb) {
	free (eb->start);
	free (eb);
}

