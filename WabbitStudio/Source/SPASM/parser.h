#ifndef __PARSER_H
#define __PARSER_H

#include "storage.h"

#define EXPR_DELIMS "+-/*&|<>%^"

extern bool parser_forward_ref_err;

bool parse_num (const char *expr, int *value);
char *parse_define (define_t *define);
int parse_f (const char *expr);
int conv_hex (const char *start, const char *end);
#ifdef WIN32
char *strndup(const char *, int len);
#endif
#endif
