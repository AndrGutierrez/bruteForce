
#include "utils.h"
#include "semaforo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

int read_user_hashes(const char *filename, UserHash **users, int *count) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
    return -1;
  }

  int lines = 0;
  char buffer[512];
  while (fgets(buffer, sizeof(buffer), file)) {
    if (strstr(buffer, "::")) {
      lines++;
    }
  }

  if (lines == 0) {
    fclose(file);
    fprintf(stderr, "No valid entries found in file\n");
    return -1;
  }

  if (lines > 50) {
    fclose(file);
    fprintf(stderr,
            "Error: Input file contains %d entries, maximum allowed is 50\n",
            lines);
    return -1;
  }

  *users = malloc(lines * sizeof(UserHash));
  if (!*users) {
    fclose(file);
    fprintf(stderr, "Memory allocation failed\n");
    return -1;
  }

  rewind(file);
  int i = 0;
  while (fgets(buffer, sizeof(buffer), file)) {
    char *sep = strstr(buffer, "::");
    if (!sep)
      continue;

    *sep = '\0';
    strncpy((*users)[i].username, buffer, sizeof((*users)[i].username) - 1);
    (*users)[i].username[sizeof((*users)[i].username) - 1] = '\0';

    char *hash_start = sep + 2;
    char *newline = strchr(hash_start, '\n');
    if (newline)
      *newline = '\0';

    strncpy((*users)[i].hash, hash_start, sizeof((*users)[i].hash) - 1);
    (*users)[i].hash[sizeof((*users)[i].hash) - 1] = '\0';

    i++;
  }

  *count = lines;
  fclose(file);
  return 0;
}

void process_user_hashes(UserHash *users, int count) {
  for (int i = 0; i < count; i++) {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    printf("\nProcessing password for user: %s\n", users[i].username);

    char *fake_argv[] = {"", users[i].hash};

    int result = semaforo(fake_argv);

    gettimeofday(&end, NULL);
    double time_taken =
        (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    print_processing_time(users[i].username, time_taken);

    if (result != 0) {
      printf("Warning: semaforo returned non-zero status for user %s\n",
             users[i].username);
    }
  }
}

void print_processing_time(const char *username, double time_taken) {
  printf("Completed user '%s' in %.6f seconds\n", username, time_taken);
}
