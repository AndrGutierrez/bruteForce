#include <openssl/evp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LENGTH 4 // Maximum password length to try

bool brute_force_md5(const char *target_hash, int length) {
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

  bool found = false;
  EVP_MD_CTX *ctx = EVP_MD_CTX_new();
  const EVP_MD *md = EVP_md5();
  unsigned char digest[EVP_MAX_MD_SIZE];
  unsigned int digest_len;

  // cada hilo buscará 1242497
  while (!found) {

    for (int i = 0; i < length; i++) {
      guess[i] = charset[indices[i]];
      // TODO: cada hilo buscará 1242497
      // aca están como los numeros de letra a los que accede, usa esto para
      // dividir el trabajo
      printf("#### %d\n", indices[i]);
    }
    guess[length] = '\0';

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
  }

  EVP_MD_CTX_free(ctx);
  free(guess);
  free(indices);
  return found;
}

bool brute_force(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <md5_hash>\n", argv[0]);
    return 1;
  }

  const char *target_hash = argv[1];

  printf("Attempting to brute force MD5 hash: %s\n", target_hash);

  for (int length = 1; length <= MAX_LENGTH; length++) {
    printf("Trying passwords of length %d...\n", length);
    if (brute_force_md5(target_hash, length)) {
      return 0;
    }
  }

  printf("Password not found for lengths 1-%d\n", MAX_LENGTH);
  return 1;
}

void *test(void *begin) {
  int i, start;
  start = *((int *)begin); // 64 bits
  for (int i = start; i <= 10; i++) {
    printf("%d \n", i);
  }
};
