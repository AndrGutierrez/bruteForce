#include "brute_force.h"
#include <errno.h>
#include <math.h> // For pow()
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants
const int MAX_THREADS = 6;
const int CHAR_SET_SIZE = 52; // a-z + A-Z
const int PASSWORD_LENGTH = 4;
const long TOTAL_COMBINATIONS = 7454980;

// Global variables
int sum = 0, numero;
pthread_mutex_t mutex_acceso;

// Function to calculate breakpoints and batch sizes
void calculate_breakpoints(int breakpoints[][4], int batch_sizes[],
                           int num_threads) {
  const long total = TOTAL_COMBINATIONS;
  const long base = CHAR_SET_SIZE;
  const long pow3 = (long)pow(base, 3); // 52^3 = 140,608
  const long pow2 = (long)pow(base, 2); // 52^2 = 2,704
  const long pow1 = base;               // 52

  // Calculate base batch size and remainder
  long base_batch = total / num_threads;
  long remainder = total % num_threads;

  for (int i = 0; i < num_threads; i++) {
    // Calculate batch size for this thread
    long batch_size = base_batch;
    if (i < remainder) {
      batch_size++;
    }
    batch_sizes[i] = (int)batch_size;

    // Calculate start index
    long start_index = i * base_batch + (i < remainder ? i : remainder);

    // Convert index to base-52 digits
    long index = start_index;
    breakpoints[i][0] = (int)(index / pow3);
    index %= pow3;
    breakpoints[i][1] = (int)(index / pow2);
    index %= pow2;
    breakpoints[i][2] = (int)(index / pow1);
    breakpoints[i][3] = (int)(index % pow1);
  }
}

// Thread function
void *runner(void *param); // Declaration

int semaforo(char *argv[], int num_threads) {
  // Validate thread count
  if (num_threads <= 0 || num_threads > MAX_THREADS) {
    fprintf(stderr,
            "Error: Invalid number of threads (%d). Must be between 1 and %d\n",
            num_threads, MAX_THREADS);
    return -1;
  }

  // Allocate memory for threads and arguments
  pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
  if (!threads) {
    fprintf(stderr, "Error: Failed to allocate memory for threads: %s\n",
            strerror(errno));
    return -1;
  }

  ThreadArgs *thread_args = malloc(num_threads * sizeof(ThreadArgs));
  if (!thread_args) {
    fprintf(stderr,
            "Error: Failed to allocate memory for thread arguments: %s\n",
            strerror(errno));
    free(threads);
    return -1;
  }

  // Initialize mutex and attributes
  pthread_attr_t attr;
  int err;

  if ((err = pthread_mutex_init(&mutex_acceso, NULL)) != 0) {
    fprintf(stderr, "Error: Failed to initialize mutex: %s\n", strerror(err));
    free(threads);
    free(thread_args);
    return -1;
  }

  if ((err = pthread_attr_init(&attr)) != 0) {
    fprintf(stderr, "Error: Failed to initialize thread attributes: %s\n",
            strerror(err));
    pthread_mutex_destroy(&mutex_acceso);
    free(threads);
    free(thread_args);
    return -1;
  }

  // Prepare breakpoints and batch sizes
  int (*breakpoints)[4] = malloc(num_threads * sizeof(int[4]));
  int *batch_sizes = malloc(num_threads * sizeof(int));
  if (!breakpoints || !batch_sizes) {
    fprintf(stderr, "Error: Failed to allocate memory for breakpoints: %s\n",
            strerror(errno));
    if (breakpoints)
      free(breakpoints);
    if (batch_sizes)
      free(batch_sizes);
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&mutex_acceso);
    free(threads);
    free(thread_args);
    return -1;
  }

  // Calculate breakpoints and batch sizes
  calculate_breakpoints(breakpoints, batch_sizes, num_threads);

  // Set up thread arguments
  atomic_store(&found, false);
  const char *username = argv[0];

  for (int i = 0; i < num_threads; i++) {
    thread_args[i].argv = &argv[1];
    thread_args[i].batch_size = batch_sizes[i];
    strncpy(thread_args[i].username, username,
            sizeof(thread_args[i].username) - 1);
    thread_args[i].username[sizeof(thread_args[i].username) - 1] = '\0';

    // Copy breakpoints for this thread
    for (int j = 0; j < 4; j++) {
      thread_args[i].start[j] = breakpoints[i][j];
    }

    // Create thread
    if ((err = pthread_create(&threads[i], &attr, brute_force,
                              (void *)&thread_args[i])) != 0) {
      fprintf(stderr, "Error: Failed to create thread %d: %s\n", i,
              strerror(err));
      // Cancel already created threads
      for (int j = 0; j < i; j++) {
        pthread_cancel(threads[j]);
      }
      // Clean up resources
      free(breakpoints);
      free(batch_sizes);
      pthread_attr_destroy(&attr);
      pthread_mutex_destroy(&mutex_acceso);
      free(threads);
      free(thread_args);
      return -1;
    }
  }

  // Free temporary arrays
  free(breakpoints);
  free(batch_sizes);

  // Wait for all threads to complete
  for (int i = 0; i < num_threads; i++) {
    if ((err = pthread_join(threads[i], NULL)) != 0) {
      fprintf(stderr, "Error: Failed to join thread %d: %s\n", i,
              strerror(err));
    }
  }

  // Cleanup
  atomic_store(&found, false);
  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&mutex_acceso);
  free(threads);
  free(thread_args);

  return 0;
}
