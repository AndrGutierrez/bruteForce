#include <openssl/evp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 4 // Maximum password length to try
#include "brute_force.h"

// int get_number_of_guesses(){
//   for()
// }
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

  // cada hilo buscará 1242497
  int guess_number[4] = {0, 0, 0, 0};
  while (!found) {

    for (int i = 0; i < length; i++) {
      guess[i] = charset[indices[i]];
      guess_number[i] = indices[i];
      // TODO: cada hilo buscará 1242497
      // aca están como los numeros de letra a los que accede, usa esto para
      // dividir el trabajo
      // printf("#### %s\n", charset);
    }
    guess[length] = '\0';
    // printf("#### %d %d %d %d\n", guess_number[0], guess_number[1],
    //        guess_number[2], guess_number[3]);
    *guesses += 1;
    // printf("%d \n", guesses);

    // printf("### %s\n", guess);
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

    // printf("#### %d\n", pos);
    if (pos == length)
      break;

    // printf("%d \n", *guesses);
    // printf("%d, %d, %d\n", *guesses % batch_size, *guesses, batch_size);
    if ((*guesses % batch_size) == 0) {
      // printf("### %d\n", *guesses);
      // printf("### %s\n", guess);
      // // printf("%d, %d\n", *guesses, start);
      // printf("#### %d %d %d %d\n", guess_number[0], guess_number[1],
      //        guess_number[2], guess_number[3]);
      // // }
      break;
    }
    // if (*guesses == 3727490) {
    //
    // printf("#### %d %d %d %d\n", guess_number[0], guess_number[1],
    //        guess_number[2], guess_number[3]);
    // }
  }

  EVP_MD_CTX_free(ctx);
  free(guess);
  free(indices);
  return found;
}

void *brute_force(void *param) {
  ThreadArgs *args = (ThreadArgs *)param;
  char **argv = args->argv;
  int batch_size = args->batch_size; // Unused (just for testing)
  int start[4];
  for (int i = 0; i < 4; i++) {
    start[i] = args->start[i];
  }

  const char *target_hash = argv[1];
  // printf("Attempting to brute force MD5 hash: %s\n", target_hash);
  // printf("Breakpoint received (unused): %d\n", breakpoint); // Debug print

  int guesses = 0;
  int init = 1;
  if (start[0] == 0 && start[1] == 0 && start[2] == 0 && start[3] == 0)
    init = 1;
  else {

    // printf("### añañin %d\n");
    init = 4;
  }
  for (int length = init; length <= MAX_LENGTH; length++) {
    // printf("Trying passwords of length %d...\n", length);
    if (brute_force_md5(target_hash, length, &guesses, batch_size, start)) {
      // printf("### guesses: %d \n", guesses);
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
