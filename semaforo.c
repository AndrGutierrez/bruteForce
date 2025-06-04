#include "brute_force.h"
#include <pthread.h>
#include <stdlib.h>

// Global variables shared by threads
int sum = 0, numero;
pthread_mutex_t mutex_acceso;

const int NUM_THREADS = 6; // Now using 5 threads
const int BATCH_SIZE = 12424967;
const int BREAKPOINTS[6][4] = {{0, 0, 0, 0},     {8, 25, 42, 7},
                               {17, 51, 33, 16}, {35, 51, 16, 34},
                               {26, 25, 25, 25}, {44, 25, 8, 43}};

void *runner(void *param); // Thread function

int semaforo(char *argv[]) {
  pthread_t threads[NUM_THREADS];
  pthread_attr_t attr;
  ThreadArgs thread_args[NUM_THREADS];

  // Initialize mutex
  pthread_mutex_init(&mutex_acceso, NULL);

  // Initialize thread attributes
  pthread_attr_init(&attr);

  // Create threads
  for (int i = 0; i < NUM_THREADS; i++) {
    thread_args[i].argv = argv;
    thread_args[i].batch_size = BATCH_SIZE;
    for (int j = 0; j < 4; j++) {
      thread_args[i].start[j] = BREAKPOINTS[i][j];
    }

    if (pthread_create(&threads[i], &attr, brute_force,
                       (void *)&thread_args[i])) {
      // perror("Failed to create thread");
      exit(EXIT_FAILURE);
    }
  }

  // Wait for all threads to complete
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], NULL);
  }

  // Clean up
  pthread_mutex_destroy(&mutex_acceso);

  return 0;
}
