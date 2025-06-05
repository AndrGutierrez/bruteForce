#ifndef BRUTE_FORCE_H
#define BRUTE_FORCE_H

#include <stdatomic.h>
#include <stdbool.h>

typedef struct {
  char **argv;
  int batch_size;
  int start[4];
  char username[256]; // Añade esto para llevar el nombre de usuario
} ThreadArgs;

extern atomic_bool found;

void *brute_force(void *param);
bool brute_force_md5(const char *target_hash, int length, int *guesses,
                     int batch_size, int start[4], const char *username);

#endif
