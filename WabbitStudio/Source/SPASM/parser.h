#ifndef __PARSER_H
#define __PARSER_H

#include "storage.h"

#define EXPR_DELIMS "+-/*&|<>%^"

extern bool parser_forward_ref_err;

bool parse_num (const char *expr, int *value);
char *parse_define (define_t *define);
int parse_f (const char *expr);
bool conv_hex (const char *start, const char *end, int *output_num);
#endif
