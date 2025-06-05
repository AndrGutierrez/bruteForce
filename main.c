#include "semaforo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

typedef struct {
  char username[256];
  char hash[33]; // MD5 hashes are 32 chars + null terminator
} UserHash;

// Function prototypes
int read_user_hashes(const char *filename, UserHash **users, int *count);
void process_user_hashes(UserHash *users, int count);
void print_processing_time(const char *username, double time_taken);

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
    char *fake_argv[] = {"", users[i].hash};
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
int read_user_hashes(const char *filename, UserHash **users, int *count) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Error opening file");
    return -1;
  }

  // First count the number of lines
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

  // Allocate memory
  *users = malloc(lines * sizeof(UserHash));
  if (!*users) {
    fclose(file);
    fprintf(stderr, "Memory allocation failed\n");
    return -1;
  }

  // Rewind and read data
  rewind(file);
  int i = 0;
  while (fgets(buffer, sizeof(buffer), file)) {
    char *sep = strstr(buffer, "::");
    if (!sep)
      continue;

    *sep = '\0'; // Split the string at ::
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

    // Prepare arguments for semaforo (it expects argv[1] to be the hash)
    char *fake_argv[] = {"", users[i].hash};

    // Call semaforo with the hash - this will block until complete
    int result = semaforo(fake_argv);

    gettimeofday(&end, NULL);
    double time_taken =
        (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

    print_processing_time(users[i].username, time_taken);

    // Print result if needed
    if (result != 0) {
      printf("Warning: semaforo returned non-zero status for user %s\n",
             users[i].username);
    }
  }
}

void print_processing_time(const char *username, double time_taken) {
  printf("Completed user '%s' in %.6f seconds\n", username, time_taken);
}
