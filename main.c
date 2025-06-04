#include "brute_force.h"
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

  // Using time() (seconds precision)
  time_t start_time, end_time;
  time(&start_time);

  // Or using gettimeofday() (microseconds precision)
  struct timeval start, end;
  gettimeofday(&start, NULL);

  semaforo(argv);

  // For time()
  time(&end_time);
  double time_taken = difftime(end_time, start_time);
  printf("Brute force took %.2f seconds (wall-clock time)\n", time_taken);

  // For gettimeofday()
  gettimeofday(&end, NULL);
  double time_taken_us =
      (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
  printf("Brute force took %.6f seconds (high precision)\n", time_taken_us);

  return 0;
}
