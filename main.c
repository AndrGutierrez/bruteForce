#include "semaforo.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <password_file>\n", argv[0]);
    return 1;
  }

  UserHash *users = NULL;
  int user_count = 0;

  if (read_user_hashes(argv[1], &users, &user_count) != 0) {
    return 1;
  }

  for (int i = 0; i < user_count; i++) {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    printf("Processing password for user: %s\n", users[i].username);

    /* semaforo expects argv[1] to be the hash */
    char *fake_argv[] = {users[i].username, users[i].hash};
    semaforo(fake_argv); /* This will block until all threads finish */

    gettimeofday(&end, NULL);
    double time_taken =
        (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    printf("Brute force for user '%s' took %.6f seconds\n", users[i].username,
           time_taken);
  }

  free(users);
  return 0;
}
