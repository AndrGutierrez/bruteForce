#ifndef BRUTE_FORCE_H
#define BRUTE_FORCE_H

#include <stdbool.h>

void *test(void *begin);
bool brute_force_md5(const char *target_hash, int length);
bool brute_force(int argc, char *argv[]);

#endif
