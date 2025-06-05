#include "brute_force.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const long long CHARSET_SIZE = 52; // a-z, A-Z

// ***** THIS IS THE CORRECTED FUNCTION *****
void index_to_breakpoint(long long index, int *bp) {
  long long len1_count = CHARSET_SIZE;
  long long len2_count = CHARSET_SIZE * CHARSET_SIZE;
  long long len3_count = CHARSET_SIZE * CHARSET_SIZE * CHARSET_SIZE;

  long long cumm_len1 = len1_count;
  long long cumm_len2 = cumm_len1 + len2_count;
  long long cumm_len3 = cumm_len2 + len3_count;

  memset(bp, 0, sizeof(int) * 4);
  long long relative_index;

  if (index < cumm_len1) { // 1-character password
    relative_index = index;
    bp[0] = (int)relative_index;
  } else if (index < cumm_len2) { // 2-character password
    relative_index = index - cumm_len1;
    bp[0] = (int)(relative_index % CHARSET_SIZE);
    bp[1] = (int)(relative_index / CHARSET_SIZE);
  } else if (index < cumm_len3) { // 3-character password
    relative_index = index - cumm_len2;
    bp[0] = (int)(relative_index % CHARSET_SIZE);
    long long temp = relative_index / CHARSET_SIZE;
    bp[1] = (int)(temp % CHARSET_SIZE);
    bp[2] = (int)(temp / CHARSET_SIZE);
  } else { // 4-character password
    relative_index = index - cumm_len3;
    bp[0] = (int)(relative_index % CHARSET_SIZE);
    long long temp = relative_index / CHARSET_SIZE;
    bp[1] = (int)(temp % CHARSET_SIZE);
    temp = temp / CHARSET_SIZE;
    bp[2] = (int)(temp % CHARSET_SIZE);
    bp[3] = (int)(temp / CHARSET_SIZE);
  }
}

void calculate_and_set_breakpoints(ThreadArgs *thread_args, int num_threads) {
  long long c1 = CHARSET_SIZE;
  long long c2 = c1 * CHARSET_SIZE;
  long long c3 = c2 * CHARSET_SIZE;
  long long c4 = c3 * CHARSET_SIZE;
  long long total_combinations = c1 + c2 + c3 + c4;

  long long batch_size = total_combinations / num_threads;

  printf("Total combinations: %lld\n", total_combinations);
  printf("Batch size per thread: %lld\n\n", batch_size);

  for (int i = 0; i < num_threads; i++) {
    long long start_index = i * batch_size;
    thread_args[i].batch_size = batch_size;

    if (i == num_threads - 1) {
      thread_args[i].batch_size = total_combinations - start_index;
    }

    index_to_breakpoint(start_index, thread_args[i].start);
  }
}

int semaforo(char *argv[], int num_threads) {
  if (num_threads <= 0) {
    fprintf(stderr, "Number of threads must be positive.\n");
    return 1;
  }

  pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
  ThreadArgs *thread_args = malloc(num_threads * sizeof(ThreadArgs));
  if (!threads || !thread_args) {
    perror("Failed to allocate memory for threads");
    free(threads);
    free(thread_args);
    exit(EXIT_FAILURE);
  }

  pthread_mutex_t mutex_acceso;
  pthread_attr_t attr;

  pthread_mutex_init(&mutex_acceso, NULL);
  pthread_attr_init(&attr);
  atomic_store(&found, false);

  const char *username = argv[0];

  calculate_and_set_breakpoints(thread_args, num_threads);

  for (int i = 0; i < num_threads; i++) {
    thread_args[i].argv = &argv[1];
    strncpy(thread_args[i].username, username,
            sizeof(thread_args[i].username) - 1);
    thread_args[i].username[sizeof(thread_args[i].username) - 1] = '\0';

    if (pthread_create(&threads[i], &attr, brute_force,
                       (void *)&thread_args[i])) {
      perror("Failed to create thread");
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }

  pthread_mutex_destroy(&mutex_acceso);
  free(threads);
  free(thread_args);

  printf("\nBrute-force finished.\n");
  return 0;
}
