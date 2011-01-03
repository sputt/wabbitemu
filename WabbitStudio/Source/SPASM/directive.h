#ifndef __DIRECTIVE_H
#define __DIRECTIVE_H

typedef enum _ES_TYPE {
	ES_BYTE,
	ES_WORD,
	ES_ECHO,
	ES_FCREATE,
} ES_TYPE;

char *handle_directive (char *ptr);
void show_define (define_t *define);
char *parse_emit_string (const char *, ES_TYPE, void *);

#endif

