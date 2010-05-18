#ifndef __PARSER_H
#define __PARSER_H

#include <stdbool.h>
#include "storage.h"

#define EXPR_DELIMS "+-/*&|<>%^"

extern bool parser_forward_ref_err;

bool parse_num (char *expr, int *value);
char *parse_define (define_t *define);
int parse_f (char *expr);
int conv_hex (const char *start, const char *end);
#ifdef WINVER
char *strndup(const char *, int len);
#endif
#endif
