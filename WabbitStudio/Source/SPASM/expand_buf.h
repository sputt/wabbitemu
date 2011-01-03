#ifndef __EXPAND_BUF_H
#define __EXPAND_BUF_H

typedef struct {
	unsigned char *start;
	unsigned char *end;
	size_t size;
} expand_buf, expand_buf_t;

expand_buf *eb_init (size_t init_size);
int eb_append(expand_buf_t *buf, const char *text, int len);
int eb_insert (expand_buf *buf, int offset, const char *text, int length);
void eb_overwrite (expand_buf *buf, int offset, const char *text, int length);
void eb_erase (expand_buf *buf, int offset, int length);
char eb_get_char (expand_buf *buf, int offset);
char *eb_extract (expand_buf *buf);
void eb_free (expand_buf *buf);

#endif

