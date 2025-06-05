#include "semaforo.h"
#include <openssl/evp.h>
#include <stdbool.h>
#include <sys/time.h> // For gettimeofday()
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <md5_hash>\n", argv[0]);
    return 1;
  }

  time_t start_time, end_time;
  time(&start_time);

  struct timeval start, end;
  gettimeofday(&start, NULL);

  semaforo(argv);

  time(&end_time);

  gettimeofday(&end, NULL);
  double time_taken_us =
      (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
  printf("Brute force took %.6f seconds \n", time_taken_us);

  return 0;
}
