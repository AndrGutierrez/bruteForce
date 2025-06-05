#ifndef BRUTE_FORCE_H
#define BRUTE_FORCE_H

#include <stdbool.h>
typedef struct {
  char **argv;    // or int numero; if you only need the parsed number
  int batch_size; // the breakpoint value to pass
  int start[4];   // the breakpoint value to pass
} ThreadArgs;

void *test(void *begin);
bool brute_force_md5(const char *target_hash, int length, int *guesses,
                     int batch_size, int start[4]);
/*void brute_force(void *argv[]);*/
void *brute_force(void *arg);
#include <stdatomic.h>
extern atomic_bool found;

#endif
