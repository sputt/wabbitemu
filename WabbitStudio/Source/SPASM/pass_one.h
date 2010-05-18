#ifndef __PASS_ONE_H
#define __PASS_ONE_H

void run_first_pass (char *ptr);
char *run_first_pass_line (char *ptr);
int write_out (int);
char *handle_opcode_or_macro (char *ptr);
void do_listing_for_line (char *ptr);

#endif
