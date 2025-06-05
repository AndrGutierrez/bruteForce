#include <openssl/evp.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 4
#include "brute_force.h"

#include <stdatomic.h>
atomic_bool found = ATOMIC_VAR_INIT(false);

bool brute_force_md5(const char *target_hash, int length, int *guesses,
                     int batch_size, int start[4]) {
  const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  const int charset_size = sizeof(charset) - 1;

  char *guess = (char *)malloc((length + 1) * sizeof(char));
  if (!guess)
    return false;

  int *indices = (int *)calloc(length, sizeof(int));
  if (!indices) {
    free(guess);
    return false;
  }

  int copy_count = (length < 4) ? length : 4;
  for (int i = 0; i < copy_count; i++) {
    indices[i] = start[i];
  }
  for (int i = copy_count; i < length; i++) {
    indices[i] = 0;
  }

  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  const EVP_MD *md = EVP_md5();
  unsigned char digest[EVP_MAX_MD_SIZE];
  unsigned int digest_len;

  while (!atomic_load(&found)) {
    if (atomic_load(&found)) {
      break;
    }

    for (int i = 0; i < length; i++) {
      guess[i] = charset[indices[i]];
    }
    guess[length] = '\0';
    (*guesses)++;

    EVP_DigestInit_ex(ctx, md, NULL);
    EVP_DigestUpdate(ctx, guess, strlen(guess));
    EVP_DigestFinal_ex(ctx, digest, &digest_len);

    char md5_str[2 * EVP_MAX_MD_SIZE + 1];
    for (unsigned int i = 0; i < digest_len; i++) {
      sprintf(&md5_str[i * 2], "%02x", (unsigned int)digest[i]);
    }

    if (strcmp(md5_str, target_hash) == 0) {
      printf("\nFound match: %s\n", guess);
      atomic_store(&found, true);
      break;
    }

    int pos = 0;
    while (pos < length) {
      indices[pos]++;
      if (indices[pos] < charset_size)
        break;
      indices[pos] = 0;
      pos++;
    }

    if (pos == length)
      break;

    if ((*guesses % batch_size) == 0 && atomic_load(&found)) {
      break;
    }
  }

  EVP_MD_CTX_free(ctx);
  free(guess);
  free(indices);
  return atomic_load(&found);
}

void *brute_force(void *param) {
  ThreadArgs *args = (ThreadArgs *)param;
  char **argv = args->argv;
  int batch_size = args->batch_size;
  int start[4];
  for (int i = 0; i < 4; i++) {
    start[i] = args->start[i];
  }

  const char *target_hash = argv[1];
  int guesses = 0;
  int init = (start[0] == 0 && start[1] == 0 && start[2] == 0 && start[3] == 0)
                 ? 1
                 : 4;

  for (int length = init; length <= MAX_LENGTH && !atomic_load(&found);
       length++) {
    brute_force_md5(target_hash, length, &guesses, batch_size, start);
  }

  pthread_exit(NULL);
}
