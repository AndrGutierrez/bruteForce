#ifndef BRUTE_FORCE_H
#define BRUTE_FORCE_H

#include <stdbool.h>
typedef struct {
  char **argv;    // or int numero; if you only need the parsed number
  int breakpoint; // the breakpoint value to pass
} ThreadArgs;

void *test(void *begin);
bool brute_force_md5(const char *target_hash, int length);
/*void brute_force(void *argv[]);*/
void *brute_force(void *arg);

#endif
