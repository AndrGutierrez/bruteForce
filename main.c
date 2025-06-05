#include "semaforo.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

void print_header(const char *text) {
  printf("\n=============================================\n");
  printf("          %s\n", text);
  printf("=============================================\n\n");
}

void print_footer(const char *text) {
  printf("\n----------------------------------------\n");
  printf("  %s\n", text);
  printf("----------------------------------------\n\n");
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "\n[!] Usage: %s <password_file>\n", argv[0]);
    return 1;
  }

  print_header("PASSWORD CRACKER INITIALIZATION");
  if (remove("passwords_found.txt") == 0) {
    printf("[+] Cleared previous results: passwords_found.txt\n");
  } else {
    printf("[*] Starting fresh output file: passwords_found.txt\n");
  }

  UserHash *users = NULL;
  int user_count = 0;

  if (read_user_hashes(argv[1], &users, &user_count) != 0) {
    return 1;
  }

  print_header("CRACKING PASSWORDS");
  for (int i = 0; i < user_count; i++) {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    printf("=== Processing: %-20s ===\n", users[i].username);

    char *fake_argv[] = {users[i].username, users[i].hash};
    semaforo(fake_argv);

    gettimeofday(&end, NULL);
    double time_taken =
        (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    printf("=> Completed in: %.6f seconds\n\n", time_taken);
  }

  print_footer("RESULTS SAVED TO: passwords_found.txt");
  free(users);
  return 0;
}
