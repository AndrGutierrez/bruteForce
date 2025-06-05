#include <openssl/evp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 4
#include "brute_force.h"

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
  // Set remaining indices to 0 if length > 4
  for (int i = copy_count; i < length; i++) {
    indices[i] = 0;
  }

  bool found = false;
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  const EVP_MD *md = EVP_md5();
  unsigned char digest[EVP_MAX_MD_SIZE];
  unsigned int digest_len;

  int guess_number[4] = {0, 0, 0, 0};
  while (!found) {

    for (int i = 0; i < length; i++) {
      guess[i] = charset[indices[i]];
      guess_number[i] = indices[i];
    }
    guess[length] = '\0';
    *guesses += 1;
    EVP_DigestInit_ex(ctx, md, NULL);
    EVP_DigestUpdate(ctx, guess, strlen(guess));
    EVP_DigestFinal_ex(ctx, digest, &digest_len);

    char md5_str[2 * EVP_MAX_MD_SIZE + 1];
    for (unsigned int i = 0; i < digest_len; i++) {
      sprintf(&md5_str[i * 2], "%02x", (unsigned int)digest[i]);
    }

    if (strcmp(md5_str, target_hash) == 0) {
      printf("\nFound match: %s\n", guess);
      found = true;
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

    if ((*guesses % 1242497) == 0) {
      printf("#### %d %d %d %d\n", guess_number[0], guess_number[1],
             guess_number[2], guess_number[3]);
      break;
    }
  }

  EVP_MD_CTX_free(ctx);
  free(guess);
  free(indices);
  return found;
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
  int init = 1;
  if (start[0] == 0 && start[1] == 0 && start[2] == 0 && start[3] == 0)
    init = 1;
  else {
    init = 4;
  }
  for (int length = init; length <= MAX_LENGTH; length++) {
    if (brute_force_md5(target_hash, length, &guesses, batch_size, start)) {
      pthread_exit(0); // Exit thread on success
    }
  }

  printf("Password not found for lengths 1-%d\n", MAX_LENGTH);
  pthread_exit((void *)1); // Exit thread on failure
}

void *test(void *begin) {
  int i, start;
  start = *((int *)begin); // 64 bits
  for (int i = start; i <= 10; i++) {
    printf("%d \n", i);
  }
};
